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

#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "SceneQuickView.h"
#include "SimpleMarkerSceneSymbol.h"

#include "DsaController.h"
#include "DsaUtility.h"
#include "DummyAlert.h"
#include "Vehicle.h"
#include "ToolResourceProvider.h"
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
  GraphicsOverlay* alertsOverlay = new GraphicsOverlay(this);
  SimpleMarkerSceneSymbol* dummySymbol = new SimpleMarkerSceneSymbol(SimpleMarkerSceneSymbolStyle::Sphere, Qt::red, 50., 50., 50., SceneSymbolAnchorPosition::Center, this);
  int alertCount = 0;
  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this, alertsOverlay, dummySymbol, &alertCount](QMouseEvent& mouseEvent)
  {
    alertCount++;
    const Point alertPos = m_sceneView->screenToBaseSurface(mouseEvent.x(), mouseEvent.y());

    qsrand(qrand());
    const int maxStatus = static_cast<int>(AlertStatus::Critical);
    const AlertStatus randomStatus = static_cast<AlertStatus>(qrand() % ( maxStatus + 1));

    Graphic* dummyAlertGraphic = new Graphic(alertPos, dummySymbol, this);
    DummyAlert* dummyAlert = new DummyAlert(dummyAlertGraphic, this);
    dummyAlert->setMessage(QString("Dummy Alert %1").arg(alertCount));
    dummyAlert->setStatus(randomStatus);
    dummyAlert->registerAlert();
    dummyAlert->setViewed(false);
    alertsOverlay->graphics()->append(dummyAlertGraphic);

    m_sceneView->graphicsOverlays()->append(alertsOverlay);
  });



  // create a GeoElementPairAlert
  // create a ProximityAlertRule
  // add a rectangle graphic
  // spin over the GraphicsOverlays in the MessageFeeds, every time a graphic is created,
  // create a new GeoElementPairAlert between
  // this and the rectangle...
  // will need to react to changes in the geo elements... trigger a re-filter
  // design wiki is saved to Documents
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
