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

#include "ArcGISTiledElevationSource.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"
#include "CoordinateConversionController.h"
#include "CoordinateConversionOptions.h"
#include "ToolManager.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "BasemapPickerController.h"

#include "DsaController.h"
#include "DsaUtility.h"
#include "Vehicle.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;
using CCO = CoordinateConversionOptions;

Vehicle::Vehicle(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(new DsaController(this))
{
  m_dataPath = DsaUtility::dataPath();
}

Vehicle::~Vehicle()
{
}

void Vehicle::componentComplete()
{
   QQuickItem::componentComplete();

  // find QML SceneView component
    m_sceneView = findChild<SceneQuickView*>("sceneView");
    connect(m_sceneView, &SceneQuickView::errorOccurred, this, &Vehicle::onError);

    // find QML Coordinate Conversion Controller object
    m_coordinateConversionController = findChild<CoordinateConversionController*>("coordinateConversionController");

    // add this tool to the tool manager since it was declared/created in Qml
    ToolManager::instance()->addTool(m_coordinateConversionController);

    // Create a map using the light grey canvas tile package
    TileCache* tileCache = new TileCache(m_dataPath + QStringLiteral("/LightGreyCanvas.tpk"), this);
    connect(tileCache, &TileCache::errorOccurred, this, &Vehicle::onError);

    // placeholder until we have ToolManager
//    for (QObject* obj : DsaUtility::tools)
//    {
//      if (!obj)
//        continue;

//      // we would add basemapChanged signal to AbstractTool and then we do not require the concrete type here
//      BasemapPickerController* basemapPicker = qobject_cast<BasemapPickerController*>(obj);
//      if (!basemapPicker)
//        continue;

//      connect(basemapPicker, &BasemapPickerController::basemapChanged, this, [this](Basemap* basemap)
//      {
//        if (!basemap)
//          return;

//        basemap->setParent(this);
//        m_scene->setBasemap(basemap);
//        connect(basemap, &Basemap::errorOccurred, this, &Vehicle::onError);
//      });
//    }

    connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
    {
      m_sceneView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    });

    connect(m_sceneView, &SceneQuickView::screenToLocationCompleted, this, [this](QUuid, Point location)
    {
      for (auto it = ToolManager::instance()->toolsBegin(); it != ToolManager::instance()->toolsEnd(); ++it)
      {
        it.value()->handleClick(location);
      }
    });

    m_scene = new Scene(this);
    connect(m_scene, &Scene::errorOccurred, this, &Vehicle::onError);

    connect(m_scene, &Scene::doneLoading, this, [this](Error)
    {
      if (m_coordinateConversionController)
        m_coordinateConversionController->setSpatialReference(m_scene->spatialReference());
    });

    // set an elevation source
    ArcGISTiledElevationSource* source = new ArcGISTiledElevationSource(QUrl(m_dataPath + "/elevation/CaDEM.tpk"), this);
    connect(source, &ArcGISTiledElevationSource::errorOccurred, this, &Vehicle::onError);
    m_scene->baseSurface()->elevationSources()->append(source);

    // Set scene to scene view
    m_sceneView->setArcGISScene(m_scene);

    // Set viewpoint to Monterey, CA
    // distance of 5000m, heading North, pitch at 75 degrees, roll of 0
    Camera monterey(DsaUtility::montereyCA(), 5000, 0., 75., 0);
    m_sceneView->setViewpointCamera(monterey);

    // set the options for the coordinateConversionTool
    setCoordinateConversionOptions();
}

void Vehicle::setCoordinateConversionOptions()
{
  if (!m_coordinateConversionController)
    return;

  CoordinateConversionOptions* option = new CoordinateConversionOptions(this);
  option->setName("Coast Gaurd");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesDecimalMinutes);

  m_coordinateConversionController->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("Air Force");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUsng);
  option->setPrecision(7);
  option->setAddSpaces(true);

  m_coordinateConversionController->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("Squad Alpha");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUtm);
  option->setUtmConversionMode(CCO::UtmConversionMode::UtmConversionModeNorthSouthIndicators);
  option->setAddSpaces(true);

  m_coordinateConversionController->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("Division B");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesMinutesSeconds);
  option->setDecimalPlaces(12);

  m_coordinateConversionController->addOption(option);
}

void Vehicle::onError(const Error&)
{
  connect(m_sceneView, &SceneQuickView::errorOccurred, m_controller, &DsaController::onError);

  m_controller->init(m_sceneView);

  // Set scene to scene view
  m_sceneView->setArcGISScene(m_controller->scene());
}
