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

// API
#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "Basemap.h"
#include "ElevationSource.h"
#include "Layer.h"
#include "GeoView.h"
#include "DictionarySymbolStyle.h"

// Toolkit
#include "ArcGISCompassController.h"
#include "CoordinateConversionController.h"
#include "ToolManager.h"

// Dsa apps
#include "DsaUtility.h"
#include "DsaController.h"
#include "AddLocalDataController.h"
#include "BasemapPickerController.h"
#include "FollowPositionController.h"
#include "LocationController.h"
#include "MessageFeedsController.h"

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

void DsaController::init(GeoView* geoView)
{
  auto toolsIt = Toolkit::ToolManager::instance()->toolsBegin();
  auto toolsEnd = Toolkit::ToolManager::instance()->toolsEnd();
  for( ; toolsIt != toolsEnd; ++toolsIt)
  {
    Toolkit::AbstractTool* abstractTool = *toolsIt;
    if (!abstractTool)
      continue;

    Toolkit::ArcGISCompassController* compassController = qobject_cast<Toolkit::ArcGISCompassController*>(abstractTool);
    if (compassController)
    {
      compassController->setView(geoView);
      continue;
    }

    BasemapPickerController* basemapPicker = qobject_cast<BasemapPickerController*>(abstractTool);
    if (basemapPicker)
    {
      connect(basemapPicker, &BasemapPickerController::basemapChanged, this, [this](Basemap* basemap)
      {
        if (!basemap)
          return;

        basemap->setParent(this);
        m_scene->setBasemap(basemap);

        connect(basemap, &Basemap::errorOccurred, this, &DsaController::onError);
      });

      continue;
    }

    AddLocalDataController* localDataController = qobject_cast<AddLocalDataController*>(abstractTool);
    if (localDataController)
    {
      connect(localDataController, &AddLocalDataController::layerSelected, this, [this](Layer* lyr)
      {
        if (!lyr)
          return;

        connect(lyr, &Layer::errorOccurred, this, &DsaController::onError);

        lyr->setParent(this);
        m_scene->operationalLayers()->append(lyr);
      });

      connect(localDataController, &AddLocalDataController::elevationSourceSelected, this, [this](ElevationSource* source)
      {
        if (!source)
          return;

        connect(source, &ElevationSource::errorOccurred, this, &DsaController::onError);

        source->setParent(this);
        m_scene->baseSurface()->elevationSources()->append(source);
      });

      continue;
    }

    LocationController* locationController = qobject_cast<LocationController*>(abstractTool);
    if (locationController)
    {
      locationController->setGpxFilePath(QUrl::fromLocalFile(m_dataPath + "/MontereyMounted.gpx"));
      geoView->graphicsOverlays()->append(locationController->locationOverlay());

      continue;
    }

    MessageFeedsController* messageFeedsController = qobject_cast<MessageFeedsController*>(abstractTool);
    if (messageFeedsController)
    {
      messageFeedsController->init(geoView);

      continue;
    }

    FollowPositionController* followController = qobject_cast<FollowPositionController*>(abstractTool);
    if (followController)
    {
      followController->init(geoView);

      continue;
    }
  }
}

void DsaController::onError(const Esri::ArcGISRuntime::Error& e)
{
  qDebug() << "Error" << e.message() << e.additionalMessage();
}
