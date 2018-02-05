// Copyright 2017 ESRI
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

#include "MarkupController.h"
#include "ToolResourceProvider.h"
#include "ToolManager.h"

#include "GeoView.h"
#include "SceneQuickView.h"
#include "Scene.h"
#include "MapQuickView.h"
#include "Map.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "MultipartBuilder.h"
#include "PolylineBuilder.h"
#include "PartCollection.h"
#include "Symbol.h"
#include "SimpleLineSymbol.h"
#include "GeometryTypes.h"
#include "GeometryEngine.h"

#include <QCursor>

using namespace Esri::ArcGISRuntime;

MarkupController::MarkupController(QObject* parent):
  AbstractSketchTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &MarkupController::updateGeoView);

  updateGeoView();
}

MarkupController::~MarkupController()
{
}

void MarkupController::setActive(bool active)
{
  if (m_active == active || !m_geoView)
    return;

  m_active = active;
  GraphicsOverlayListModel* graphicsOverlays = m_geoView->graphicsOverlays();
  active ? graphicsOverlays->append(m_sketchOverlay) : graphicsOverlays->removeOne(m_sketchOverlay);
}

void MarkupController::setDrawingAltitude(double altitude)
{
  if (m_drawingAltitude == altitude)
    return;

  m_drawingAltitude = altitude;
}

double MarkupController::drawingAltitude() const
{
  return m_drawingAltitude;
}

// creates a new LineSymbol rather than updating the current one so previously drawn sketches stay the same color
void MarkupController::setColor(QColor color)
{
  m_sketchSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, color, 8, this);
}

void MarkupController::setSurfacePlacement(int placementEnum)
{
  m_sketchOverlay->setSceneProperties(LayerSceneProperties(static_cast<SurfacePlacement>(placementEnum)));
}

void MarkupController::deleteSelectedGraphics()
{
  if (m_sketchOverlay->selectedGraphics().isEmpty())
    return;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  for (auto graphic : m_sketchOverlay->selectedGraphics())
  {
    int index = m_sketchOverlay->graphics()->indexOf(graphic);
    m_partOutlineGraphics.removeAt(index);
    multipartBuilder->parts()->removePart(index);
    m_sketchOverlay->graphics()->removeOne(graphic);
  }
}

void MarkupController::deleteAllGraphics()
{
  // remove graphics from view
  m_sketchOverlay->graphics()->clear();

  // clear stored Graphics
  m_partOutlineGraphics.clear();

  // clear GeometryBuilder
  clear();
}

void MarkupController::setDrawModeEnabled(bool enabled)
{
  if (m_drawModeEnabled == enabled)
    return;

  m_drawModeEnabled = enabled;
}

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
    m_currentPartIndex = addPart();
    Graphic* partGraphic = new Graphic(this);
    partGraphic->setSymbol(m_sketchSymbol);
    m_partOutlineGraphics.append(partGraphic);
    m_sketchOverlay->graphics()->append(partGraphic);

    Toolkit::ToolResourceProvider::instance()->setMouseCursor(QCursor(Qt::PointingHandCursor));
    m_isDrawing = true;

    Point pressedPoint(normalizedPoint(mouseEvent.x(), mouseEvent.y()));
    if (m_sketchOverlay->sceneProperties().surfacePlacement() == SurfacePlacement::Relative)
      pressedPoint = Point(pressedPoint.x(), pressedPoint.y(), m_drawingAltitude);

    insertPointInPart(m_currentPartIndex, -1, pressedPoint);

    // for touch screen operation
    mouseEvent.ignore();
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
  });
}

// to be called whenever the GeometryBuilder is modified. It will update the Geometry of the Graphic being sketched
void MarkupController::updateSketch()
{
  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  Part* currentPart = multipartBuilder->parts()->part(m_currentPartIndex);

  MultipartBuilder* outlineBuilder = new PolylineBuilder(m_geoView->spatialReference(), this);
  outlineBuilder->parts()->addPart(currentPart);

  // get simplified geometry
  const Geometry simplifiedLine = GeometryEngine::simplify(outlineBuilder->toGeometry());
  m_partOutlineGraphics.at(m_currentPartIndex)->setGeometry(simplifiedLine);
}

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

bool MarkupController::is3d() const
{
  return m_is3d;
}

int MarkupController::sketchCount() const
{
  return m_partOutlineGraphics.size();
}

bool MarkupController::drawModeEnabled() const
{
  return m_drawModeEnabled;
}

QString MarkupController::toolName() const
{
  return "Markup Tool";
}

GeometryType MarkupController::geometryType() const
{
  return GeometryType::Polyline;
}
