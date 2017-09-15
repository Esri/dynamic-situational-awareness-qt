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
#include "Scene.h"
#include "SceneQuickView.h"

#include "BasemapPickerController.h"
#include "LocationController.h"
#include "DsaUtility.h"
#include "GraphicsOverlay.h"
#include "ModelSceneSymbol.h"

#include "Vehicle.h"

using namespace Esri::ArcGISRuntime;

Vehicle::Vehicle(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
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

  // Create a scene using the light grey canvas tile package
  TileCache* tileCache = new TileCache(m_dataPath + QStringLiteral("/LightGreyCanvas.tpk"), this);
  connect(tileCache, &TileCache::errorOccurred, this, &Vehicle::onError);

  // placeholder until we have ToolManager
  for (QObject* obj : DsaUtility::tools)
  {
    if (!obj)
      continue;

    // we would add basemapChanged signal to AbstractTool and then we do not require the concrete type here
    if (qobject_cast<BasemapPickerController*>(obj))
    {
      auto basemapPicker = static_cast<BasemapPickerController*>(obj);
      connect(basemapPicker, &BasemapPickerController::basemapChanged, this, [this](Basemap* basemap)
      {
        if (!basemap)
          return;

        basemap->setParent(this);
        m_scene->setBasemap(basemap);

        connect(basemap, &Basemap::errorOccurred, this, &Vehicle::onError);
      });
    }
    else if (qobject_cast<LocationController*>(obj))
    {
      auto locationController = static_cast<LocationController*>(obj);

      auto overlay = new GraphicsOverlay(this);
      overlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
      overlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
      m_sceneView->graphicsOverlays()->append(overlay);

      ModelSceneSymbol* symbol = new ModelSceneSymbol(QUrl::fromLocalFile(DsaUtility::dataPath() + "/LocationDisplay.dae"), this);
      constexpr float symbolSize = 24.0;
      symbol->setWidth(symbolSize);
      symbol->setDepth(symbolSize);
      symbol->load();
      m_positionGraphic = new Graphic(this);
      m_positionGraphic->setSymbol(symbol);

      overlay->graphics()->append(m_positionGraphic);

      connect(locationController, &LocationController::positionChanged, this, [this](const Point& newPosition)
      {
        m_positionGraphic->setGeometry(Point(newPosition.x(), newPosition.y(), 10.0));
      });

      connect(locationController, &LocationController::headingChanged, this, [this, symbol](double newHeading)
      {
        symbol->setHeading(newHeading);
      });
    }
  }

  m_scene = new Scene(this);
  connect(m_scene, &Scene::errorOccurred, this, &Vehicle::onError);

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
}

void Vehicle::onError(const Error&)
{

}
