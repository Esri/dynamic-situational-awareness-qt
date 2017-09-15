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
#include "Scene.h"

#include "DsaUtility.h"
#include "DsaController.h"

#include "BasemapPickerController.h"
#include "LocationController.h"

using namespace Esri::ArcGISRuntime;


DsaController::DsaController(QObject* parent):
  QObject(parent),
  m_scene(new Scene(this)),
  m_dataPath(DsaUtility::dataPath())
{
  // Set viewpoint to Monterey, CA
  // distance of 5000m, heading North, pitch at 75 degrees, roll of 0
  Camera monterey(DsaUtility::montereyCA(), 5000, 0., 75., 0);
  Viewpoint initViewpoint(DsaUtility::montereyCA(), monterey);
  m_scene->setInitialViewpoint(initViewpoint);

  connect(m_scene, &Scene::errorOccurred, this, &DsaController::onError);

  // set an elevation source
  ArcGISTiledElevationSource* source = new ArcGISTiledElevationSource(QUrl(m_dataPath + "/elevation/CaDEM.tpk"), this);
  connect(source, &ArcGISTiledElevationSource::errorOccurred, this, &DsaController::onError);
  m_scene->baseSurface()->elevationSources()->append(source);
}

DsaController::~DsaController()
{
}

Esri::ArcGISRuntime::Scene* DsaController::scene() const
{
  return m_scene;
}

void DsaController::init()
{
  // placeholder until we have ToolManager
  for (QObject* obj : DsaUtility::tools)
  {
    if (!obj)
      continue;

    // we would add basemapChanged signal to AbstractTool and then we do not require the concrete type here
    if (qobject_cast<BasemapPickerController*>(obj))
    {
      BasemapPickerController* basemapPicker = static_cast<BasemapPickerController*>(obj);
      connect(basemapPicker, &BasemapPickerController::basemapChanged, this, [this](Basemap* basemap)
      {
        if (!basemap)
          return;

        basemap->setParent(this);
        m_scene->setBasemap(basemap);

        connect(basemap, &Basemap::errorOccurred, this, &DsaController::onError);
      });
    }
    else if (qobject_cast<LocationController*>(obj))
    {
      LocationController* locationController = static_cast<LocationController*>(obj);
      m_overlays.append(locationController->locationOverlay());
    }
  }
}

QList<GraphicsOverlay*> DsaController::overlays() const
{
  return m_overlays;
}

void DsaController::onError(const Esri::ArcGISRuntime::Error&)
{
}
