// Copyright 2016 ESRI
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

#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "PolygonBuilder.h"
#include "SceneQuickView.h"
#include "SimpleLineSymbol.h"

#include "DsaController.h"
#include "Handheld.h"
#include "ToolResourceProvider.h"
#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

Handheld::Handheld(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(new DsaController(this))
{
}

Handheld::~Handheld()
{
}

void Handheld::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  connect(m_sceneView, &SceneQuickView::errorOccurred, m_controller, &DsaController::onError);

  m_controller->init(m_sceneView);

  // connect to the DSA controller errors
  connect(m_controller, &DsaController::errorOccurred, this, [this]
          (const QString& message, const QString& additionalMessage)
  {
    emit errorOccurred(message, additionalMessage);
  });


  // setup the connections from the view to the resource provider
  connect(m_sceneView, &SceneQuickView::spatialReferenceChanged,
          ToolResourceProvider::instance(), &ToolResourceProvider::spatialReferenceChanged);

  connect(m_sceneView, &SceneQuickView::mouseClicked,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMouseClicked);

  connect(m_sceneView, &SceneQuickView::mousePressed,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMousePressed);

  connect(m_sceneView, &SceneQuickView::mouseMoved,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMouseMoved);

  connect(m_sceneView, &SceneQuickView::mouseReleased,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMouseReleased);

  connect(m_sceneView, &SceneQuickView::identifyGraphicsOverlayCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyGraphicsOverlayCompleted);

  connect(m_sceneView, &SceneQuickView::identifyGraphicsOverlaysCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyGraphicsOverlaysCompleted);

  connect(m_sceneView, &SceneQuickView::identifyLayerCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyLayerCompleted);

  connect(m_sceneView, &SceneQuickView::identifyLayersCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyLayersCompleted);

  connect(m_sceneView, &SceneQuickView::screenToLocationCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onScreenToLocationCompleted);

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::setMouseCursorRequested, this, [this](const QCursor& mouseCursor)
  {
    m_sceneView->setCursor(mouseCursor);
  });

  // Set scene to scene view
  m_sceneView->setArcGISScene(m_controller->scene());

  GraphicsOverlay* geofenceOverlay = new GraphicsOverlay(this);
  geofenceOverlay->setOverlayId("Geofence overlay");
  SimpleLineSymbol* geofenceSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::green, 5, this);
  PolygonBuilder pb(SpatialReference::wgs84());
  pb.addPoint(-121.91, 36.605);
  pb.addPoint(-121.91, 36.61);
  pb.addPoint(-121.92, 36.61);
  pb.addPoint(-121.92, 36.605);

  Graphic* geofenceGraphic = new Graphic(pb.toPolygon(), geofenceSymbol, this);
  geofenceGraphic->attributes()->insertAttribute("Test Attribute", "Test Value");
  geofenceOverlay->graphics()->append(geofenceGraphic);
  m_sceneView->graphicsOverlays()->append(geofenceOverlay);
}
