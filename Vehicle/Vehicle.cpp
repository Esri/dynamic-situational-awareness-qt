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

#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "PolygonBuilder.h"
#include "SceneQuickView.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleLineSymbol.h"

#include "AlertListModel.h"
#include "DsaController.h"
#include "DsaUtility.h"
#include "DummyAlert.h"
#include "GraphicPairAlert.h"
#include "LocationController.h"
#include "Vehicle.h"
#include "ToolResourceProvider.h"
#include "ToolManager.h"
#include "CoordinateConversionController.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;
using CCO = CoordinateConversionOptions;

Vehicle::Vehicle(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(new DsaController(this))
{
}

Vehicle::~Vehicle()
{
}

void Vehicle::componentComplete()
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

  // set the options for the coordinateConversionTool
  setCoordinateConversionOptions();

  // Alerts: Proof of concept
  DictionarySymbolStyle* dictionarySymbolStyle = new DictionarySymbolStyle("mil2525d", this);
  DictionaryRenderer* renderer = new DictionaryRenderer(dictionarySymbolStyle, this);

  GraphicsOverlay* alertsOverlay = new GraphicsOverlay(this);
  alertsOverlay->setRenderer(renderer);
    alertsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
//  SimpleMarkerSceneSymbol* dummySymbol = new SimpleMarkerSceneSymbol(SimpleMarkerSceneSymbolStyle::Sphere, Qt::red, 50., 50., 50., SceneSymbolAnchorPosition::Center, this);
  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this, alertsOverlay](QMouseEvent& mouseEvent)
  {
    const Point alertPos = m_sceneView->screenToBaseSurface(mouseEvent.x(), mouseEvent.y());
    const int alertCount = AlertListModel::instance()->rowCount();

    qsrand(qrand());
    const int maxStatus = static_cast<int>(AlertStatus::Critical);
    AlertStatus randomStatus = static_cast<AlertStatus>((qrand() % ( maxStatus + 1) - 1) + 1);
    if (randomStatus == AlertStatus::Inactive)
      randomStatus = AlertStatus::Low;

    Graphic* dummyAlertGraphic = new Graphic(alertPos, this);
    dummyAlertGraphic->attributes()->insertAttribute("sic", "GFGPOAO-------X");
    DummyAlert* dummyAlert = new DummyAlert(dummyAlertGraphic, alertsOverlay, this);
    dummyAlert->setMessage(QString("Dummy Alert %1").arg(alertCount));
    dummyAlert->setStatus(randomStatus);
    dummyAlert->setViewed(false);
    dummyAlert->setActive(true);
    alertsOverlay->graphics()->append(dummyAlertGraphic);
    dummyAlert->registerAlert();

    m_sceneView->graphicsOverlays()->append(alertsOverlay);
  });

  GraphicsOverlay* geofenceOverlay = new GraphicsOverlay(this);
  SimpleLineSymbol* geofenceSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::green, 5, this);
  PolygonBuilder pb(SpatialReference::wgs84());
  pb.addPoint(-121.91, 36.605);
  pb.addPoint(-121.91, 36.61);
  pb.addPoint(-121.92, 36.61);
  pb.addPoint(-121.92, 36.605);

  Graphic* geofenceGraphic = new Graphic(pb.toPolygon(), geofenceSymbol, this);
  geofenceOverlay->graphics()->append(geofenceGraphic);
  m_sceneView->graphicsOverlays()->append(geofenceOverlay);

  LocationController* locationTool = Toolkit::ToolManager::instance().tool<LocationController>();
  if (locationTool)
  {
    Graphic* locationGraphic = locationTool->positionGraphic();
    if (locationGraphic)
    {
      GraphicPairAlert* geofenceAlert = new GraphicPairAlert(locationGraphic, geofenceGraphic, 0., this);
      geofenceAlert->setStatus(AlertStatus::Critical);
      geofenceAlert->setMessage("Location in geofence");
      geofenceAlert->registerAlert();
      geofenceAlert->setViewed(false);

      connect(locationTool, &LocationController::positionChanged, geofenceAlert, &GraphicPairAlert::onPositionChanged);
    }
  }
}

void Vehicle::setCoordinateConversionOptions()
{
  CoordinateConversionController* controller = findChild<CoordinateConversionController*>("coordinateConversionController");

  if (!controller)
    return;

  CoordinateConversionOptions* option = new CoordinateConversionOptions(this);
  option->setName("DDM");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesDecimalMinutes);

  controller->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("USNG");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUsng);
  option->setPrecision(7);
  option->setAddSpaces(true);

  controller->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("UTM");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUtm);
  option->setUtmConversionMode(CCO::UtmConversionMode::UtmConversionModeNorthSouthIndicators);
  option->setAddSpaces(true);

  controller->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("DMS");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesMinutesSeconds);
  option->setDecimalPlaces(12);

  controller->addOption(option);
}
