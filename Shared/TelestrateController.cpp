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

#include "TelestrateController.h"
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

#include <QCursor>

using namespace Esri::ArcGISRuntime;

TelestrateController::TelestrateController(QObject* parent):
  SketchTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &TelestrateController::updateGeoView);

  updateGeoView();
}

TelestrateController::~TelestrateController()
{
}

void TelestrateController::setActive(bool active)
{
  if (m_active == active || !m_geoView)
    return;

  m_active = active;
  GraphicsOverlayListModel* graphicsOverlays = m_geoView->graphicsOverlays();
  active ? graphicsOverlays->append(m_sketchOverlay) : graphicsOverlays->removeOne(m_sketchOverlay);
}

// creates a new LineSymbol rather than updating the current one so previously drawn sketches stay the same color
void TelestrateController::setColor(QColor color)
{
  m_sketchSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, color, 8, this);
}

void TelestrateController::setSurfacePlacement(int placementEnum)
{
  m_sketchOverlay->setSceneProperties(LayerSceneProperties(static_cast<SurfacePlacement>(placementEnum)));
}

void TelestrateController::deleteSelectedGraphics()
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

  // in 3D, reset the GraphicsOverlay because of Dynamic Rendering issues
  if (m_is3d)
    refreshSketchLayer();
}

void TelestrateController::deleteAllGraphics()
{
  // remove graphics from view
  m_sketchOverlay->graphics()->clear();

  // clear stored Graphics
  m_partOutlineGraphics.clear();

  // clear GeometryBuilder
  clear();

  // in 3D, reset the GraphicsOverlay because of Dynamic Rendering issues
  if (m_is3d)
    refreshSketchLayer();
}

void TelestrateController::setDrawModeEnabled(bool enabled)
{
  if (m_drawModeEnabled == enabled)
    return;

  m_drawModeEnabled = enabled;
}

void TelestrateController::init()
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

    if (m_is3d)
      static_cast<SceneQuickView*>(m_geoView)->setCursor(QCursor(Qt::PointingHandCursor));
    else
      static_cast<MapQuickView*>(m_geoView)->setCursor(QCursor(Qt::PointingHandCursor));

    m_isDrawing = true;

    Point pressedPoint(normalizedPoint(mouseEvent.x(), mouseEvent.y()));
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
    insertPointInPart(m_currentPartIndex, -1, movedPoint);
  });

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseReleased, this, [this](QMouseEvent& mouseEvent)
  {
    if (!m_active || !m_isDrawing)
      return;

    mouseEvent.accept();

    Point releasedPoint(normalizedPoint(mouseEvent.x(), mouseEvent.y()));
    insertPointInPart(m_currentPartIndex, -1, releasedPoint);

    if (m_is3d)
      static_cast<SceneQuickView*>(m_geoView)->setCursor(QCursor(Qt::ArrowCursor));
    else
      static_cast<MapQuickView*>(m_geoView)->setCursor(QCursor(Qt::ArrowCursor));

    m_isDrawing = false;
  });
}

// workaround for issue where deleted Graphics don't get cleared from View. Removes and reappends the GraphicsOverlay
void TelestrateController::refreshSketchLayer()
{
  if (!m_geoView)
    return;

  if (m_geoView->graphicsOverlays()->contains(m_sketchOverlay))
  {
    m_geoView->graphicsOverlays()->removeOne(m_sketchOverlay);
    m_geoView->graphicsOverlays()->append(m_sketchOverlay);
  }
  else
  {
    m_geoView->graphicsOverlays()->append(m_sketchOverlay);
    m_geoView->graphicsOverlays()->removeOne(m_sketchOverlay);
  }
}

// to be called whenever the GeometryBuilder is modified. It will update the Geometry of the Graphic being sketched
void TelestrateController::updateSketch()
{
  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  Part* currentPart = multipartBuilder->parts()->part(m_currentPartIndex);

  MultipartBuilder* outlineBuilder = new PolylineBuilder(m_geoView->spatialReference(), this);
  outlineBuilder->parts()->addPart(currentPart);
  m_partOutlineGraphics.at(m_currentPartIndex)->setGeometry(outlineBuilder->toGeometry());
}

void TelestrateController::updateGeoView()
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

bool TelestrateController::is3d() const
{
  return m_is3d;
}

int TelestrateController::sketchCount() const
{
  return m_partOutlineGraphics.size();
}

bool TelestrateController::drawModeEnabled() const
{
  return m_drawModeEnabled;
}

QString TelestrateController::toolName() const
{
  return "Telestrate Tool";
}

GeometryType TelestrateController::geometryType() const
{
  return GeometryType::Polyline;
}
