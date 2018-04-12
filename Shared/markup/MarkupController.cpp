// Copyright 2018 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

// PCH header
#include "pch.hpp"

#include "MarkupController.h"

// example app headers
#include "MarkupBroadcast.h"
#include "MarkupLayer.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "FeatureCollectionLayer.h"
#include "GeoView.h"
#include "GeometryEngine.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MultipartBuilder.h"
#include "PartCollection.h"
#include "PolylineBuilder.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SimpleLineSymbol.h"
#include "Symbol.h"

// Qt headers
#include <QCursor>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString MarkupController::USERNAME_PROPERTYNAME = "UserName";

/*!
  \class MarkupController
  \inherits AbstractSketchTool
  \brief Tool controller for creating markups.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
MarkupController::MarkupController(QObject* parent):
  AbstractSketchTool(parent),
  m_markupBroadcast(new MarkupBroadcast(parent))
{
  Toolkit::ToolManager::instance().addTool(this);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &MarkupController::updateGeoView);

  updateGeoView();
  updatedSymbol();

  connect(m_markupBroadcast, &MarkupBroadcast::markupReceived, this, [this](const QString& fileName, const QString& sharedBy)
  {
    emit this->markupReceived(fileName, sharedBy);
  });

  connect(m_markupBroadcast, &MarkupBroadcast::markupSent, this, [this](const QString& fileName)
  {
    emit this->markupSent(fileName);
  });
}

/*!
 \brief Destructor.
 */
MarkupController::~MarkupController()
{
}

/*!
 \brief Sets \a properties from the configuration file
 */
void MarkupController::setProperties(const QVariantMap& properties)
{
  m_username = properties.value(USERNAME_PROPERTYNAME).toString();
}

/*!
 \brief Sets the tool to be \a active.
 */
void MarkupController::setActive(bool active)
{
  if (m_active == active || !m_geoView)
    return;

  m_active = active;
  GraphicsOverlayListModel* graphicsOverlays = m_geoView->graphicsOverlays();
  if (active)
    graphicsOverlays->append(m_sketchOverlay);
  emit activeChanged();
}

/*!
 \brief Sets the drawing altitude to \a altitude.
 */
void MarkupController::setDrawingAltitude(double altitude)
{
  if (m_drawingAltitude == altitude)
    return;

  m_drawingAltitude = altitude;
}

/*!
 \brief Returns the drawing altitude.
 */
double MarkupController::drawingAltitude() const
{
  return m_drawingAltitude;
}

/*!
 \brief Sets the color for the markup to \a color.
 */
void MarkupController::setColor(const QColor& color)
{
  if (m_color == color)
    return;

  m_color = color;
}

/*!
 \brief Sets the width for the markup to \a width.
 */
void MarkupController::setWidth(float width)
{
  if (m_width == width)
    return;

  m_width = width;
}

/*!
 \brief Returns a new \l Esri::ArcGISRuntime::Symbol using the current color and width.
 */
Symbol* MarkupController::updatedSymbol()
{
  m_sketchSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, m_color, m_width, this);
  auto lineSym = dynamic_cast<SimpleLineSymbol*>(sketchSymbol());
  if (lineSym)
    lineSym->setAntiAlias(true);

  return m_sketchSymbol;
}

/*!
 \brief Sets the surface placement mode for the markup to \a placementEnum.

 \sa Esri::ArcGISRuntime::SurfacePlacement
 */
void MarkupController::setSurfacePlacement(int placementEnum)
{
  m_sketchOverlay->setSceneProperties(LayerSceneProperties(static_cast<SurfacePlacement>(placementEnum)));
}

/*!
 \brief Deletes the selected graphics.
 */
void MarkupController::deleteSelectedGraphics()
{
  if (m_sketchOverlay->selectedGraphics().isEmpty())
    return;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  const auto graphics = m_sketchOverlay->selectedGraphics();
  for (auto graphic : graphics)
  {
    int index = m_sketchOverlay->graphics()->indexOf(graphic);
    m_partOutlineGraphics.removeAt(index);
    multipartBuilder->parts()->removePart(index);
    m_sketchOverlay->graphics()->removeOne(graphic);
    delete graphic;
  }
}

/*!
 \brief Deletes all graphics.
 */
void MarkupController::deleteAllGraphics()
{
  // remove graphics from view
  m_sketchOverlay->graphics()->clear();

  // clear stored Graphics
  m_partOutlineGraphics.clear();
  qDeleteAll(m_partOutlineGraphics);

  // clear GeometryBuilder
  clear();
  m_currentPartIndex = 0;
}

/*!
 \brief Sets whether drawing is enabled to \a enabled.
 */
void MarkupController::setDrawModeEnabled(bool enabled)
{
  if (m_drawModeEnabled == enabled)
    return;

  m_drawModeEnabled = enabled;
}

/*!
 \internal
 */
void MarkupController::init()
{
  initGeometryBuilder();

  if (m_is3d)
    m_sketchOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Draped));

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlayCompleted, this, [this](QUuid, IdentifyGraphicsOverlayResult* identifyResult)
  {
    if (!m_active)
      return;

    if (identifyResult->graphics().size() > 0)
      identifyResult->graphicsOverlay()->selectGraphics(identifyResult->graphics());
    else
      m_sketchOverlay->unselectGraphics(m_sketchOverlay->selectedGraphics());
  });

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if (!m_active)
      return;

    if (!m_isDrawing)
      m_geoView->identifyGraphicsOverlay(m_sketchOverlay, mouseEvent.x(), mouseEvent.y(), m_is3d ? 100 : 20, false, 1);
  });

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mousePressed, this, [this](QMouseEvent& mouseEvent)
  {
    // ignore right clicks
    if (mouseEvent.button() == Qt::MouseButton::RightButton)
      return;

    // do nothing if Tool is not active
    if (!m_active || !m_drawModeEnabled)
      return;

    // accept mouseEvent when using a mouse device to disable panning.
    if (mouseEvent.button() == Qt::MouseButton::LeftButton)
      mouseEvent.accept();

    // create a new graphic that corresponds to a new Part of the GeometryBuilder
    clear();
    m_currentPartIndex = 0;
    Graphic* partGraphic = new Graphic(this);
    partGraphic->setSymbol(updatedSymbol());
    m_partOutlineGraphics.append(partGraphic);
    m_sketchOverlay->graphics()->append(partGraphic);
    m_currentPartIndex = addPart();

    Point pressedPoint(normalizedPoint(mouseEvent.x(), mouseEvent.y()));
    if (m_sketchOverlay->sceneProperties().surfacePlacement() == SurfacePlacement::Relative)
      pressedPoint = Point(pressedPoint.x(), pressedPoint.y(), m_drawingAltitude);

    insertPointInPart(m_currentPartIndex, -1, pressedPoint);

    // for touch screen operation
    mouseEvent.ignore();

    Toolkit::ToolResourceProvider::instance()->setMouseCursor(QCursor(Qt::PointingHandCursor));
    m_isDrawing = true;
  });

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseMoved, this, [this](QMouseEvent& mouseEvent)
  {
    if (!m_active || !m_isDrawing)
      return;

    mouseEvent.accept();

    Point movedPoint(normalizedPoint(mouseEvent.x(), mouseEvent.y()));
    if (m_sketchOverlay->sceneProperties().surfacePlacement() == SurfacePlacement::Relative)
      movedPoint = Point(movedPoint.x(), movedPoint.y(), m_drawingAltitude);

    insertPointInPart(m_currentPartIndex, -1, movedPoint);
  });

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseReleased, this, [this](QMouseEvent& mouseEvent)
  {
    if (!m_active || !m_isDrawing)
      return;

    mouseEvent.accept();

    Point releasedPoint(normalizedPoint(mouseEvent.x(), mouseEvent.y()));
    if (m_sketchOverlay->sceneProperties().surfacePlacement() == SurfacePlacement::Relative)
      releasedPoint = Point(releasedPoint.x(), releasedPoint.y(), m_drawingAltitude);

    insertPointInPart(m_currentPartIndex, -1, releasedPoint);

    Toolkit::ToolResourceProvider::instance()->setMouseCursor(QCursor(Qt::ArrowCursor));
    m_isDrawing = false;

    emit sketchCompleted();
  });
}

/*!
 \internal
 */
void MarkupController::updateSketch()
{
  // to be called whenever the GeometryBuilder is modified. It will update the Geometry of the Graphic being sketched
  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);

  // get simplified geometry
  const Geometry simplifiedLine = GeometryEngine::simplify(multipartBuilder->toGeometry());
  auto graphic = m_partOutlineGraphics.last();
  if (!graphic)
    return;

  graphic->setSymbol(m_sketchSymbol);
  graphic->setGeometry(simplifiedLine);
}

/*!
 \internal
 */
void MarkupController::updateGeoView()
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();

  if (!geoView)
    return;

  if (geoView->geoViewType() == GeoViewType::Unknown)
    return;

  m_geoView = geoView;

  m_is3d = geoView->geoViewType() == GeoViewType::SceneView;
  emit is3dChanged();

  init();
}

/*!
 \brief Returns whether the app is 3D.
 */
bool MarkupController::is3d() const
{
  return m_is3d;
}

/*!
 \brief Returns the number of sketches.
 */
int MarkupController::sketchCount() const
{
  return m_partOutlineGraphics.size();
}

/*!
 \brief Returns whether drawing is enabled.
 */
bool MarkupController::drawModeEnabled() const
{
  return m_drawModeEnabled;
}

/*!
 \brief Returns the name of this tool.
 */
QString MarkupController::toolName() const
{
  return "Markup Tool";
}

/*!
 \brief Returns the type of geometry. This will always be \c Polyline.
 */
GeometryType MarkupController::geometryType() const
{
  return GeometryType::Polyline;
}

/*!
 \brief Sets the overlay name to \a name.
 */
void MarkupController::setOverlayName(const QString& name)
{
  if (!m_sketchOverlay)
    return;

  if (m_sketchOverlay->overlayId() == name)
    return;

  QString overlayId = name.length() > 0 ? name : "Markup";
  m_sketchOverlay->setOverlayId(overlayId);
}

/*!
 \brief Returns the list of colors available for markups.
 */
QStringList MarkupController::colors() const
{
  return MarkupLayer::colors();
}

/*!
 \brief Creates and braodcasts a new \l MarkupLayer from the current sketch.
 */
void MarkupController::shareMarkup()
{
  if (!m_markupBroadcast)
    return;

  MarkupLayer* markupLayer = MarkupLayer::createFromGraphics(sketchOverlay(), m_username, this);
  m_markupBroadcast->broadcastMarkup(markupLayer->toJson());
}

/*!
 \brief Returns the current markup color.
 */
QColor MarkupController::currentColor() const
{
  return m_color;
}

} // Dsa
