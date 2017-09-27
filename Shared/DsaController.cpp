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

// Qt
#include <QSettings>
#include <QDir>

using namespace Esri::ArcGISRuntime;

DsaController::DsaController(QObject* parent):
  QObject(parent),
  m_scene(new Scene(this))
{
  // setup config settings
  setupConfig();
  m_dataPath = m_dsaSettings["RootDataDirectory"].toString();

  // Set viewpoint to Monterey, CA
  // distance of 5000m, heading North, pitch at 75 degrees, roll of 0
  Camera monterey(DsaUtility::montereyCA(), 5000, 0., 75., 0);
  Viewpoint initViewpoint(DsaUtility::montereyCA(), monterey);
  m_scene->setInitialViewpoint(initViewpoint);

  connect(m_scene, &Scene::errorOccurred, this, &DsaController::onError);

  // set an elevation source
  ArcGISTiledElevationSource* source = new ArcGISTiledElevationSource(QUrl(m_dsaSettings["DefaultElevationSource"].toString()), this);
  connect(source, &ArcGISTiledElevationSource::errorOccurred, this, &DsaController::onError);
  m_scene->baseSurface()->elevationSources()->append(source);
}

DsaController::~DsaController()
{
  // save the settings
  QFile configFile(m_configFilePath);
  saveSettings(configFile);
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
      basemapPicker->setDefaultBasemap(m_dsaSettings["DefaultBasemap"].toString());
      basemapPicker->setBasemapDataPath(m_dsaSettings["BasemapDirectory"].toString());
      connect(basemapPicker, &BasemapPickerController::basemapChanged, this, [this](Basemap* basemap, QString name)
      {
        if (!basemap)
          return;

        m_dsaSettings["DefaultBasemap"] = name;

        basemap->setParent(this);
        m_scene->setBasemap(basemap);

        connect(basemap, &Basemap::errorOccurred, this, &DsaController::onError);
      });

      continue;
    }

    AddLocalDataController* localDataController = qobject_cast<AddLocalDataController*>(abstractTool);
    if (localDataController)
    {
      for (const QString& filePath : m_dsaSettings["LocalDataPaths"].toStringList())
      {
        localDataController->addPathToDirectoryList(filePath);
      }
      localDataController->refreshLocalDataModel();

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
      bool simulate = QString::compare(m_dsaSettings["SimulateLocation"].toString(), QString("true"), Qt::CaseInsensitive) == 0;
      locationController->setSimulated(simulate);
      locationController->setGpxFilePath(QUrl::fromLocalFile(m_dsaSettings["GpxFile"].toString()));
      locationController->setIconDataPath(m_dsaSettings["ResourceDirectory"].toString());

      geoView->graphicsOverlays()->append(locationController->locationOverlay());

      continue;
    }

    MessageFeedsController* messageFeedsController = qobject_cast<MessageFeedsController*>(abstractTool);
    if (messageFeedsController)
    {
      messageFeedsController->setDataPath(m_dsaSettings["ResourceDirectory"].toString());
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

void DsaController::setupConfig()
{
  // create the default settings map
  createDefaultSettings();

  // get the app config
  m_configFilePath = QString("%1/%2").arg(m_dsaSettings["RootDataDirectory"].toString(), "DsaAppConfig.ini");

  // If the config file does not exist, create it, and set all of the defaults
  QFile configFile(m_configFilePath);
  if (!configFile.exists())
  {
    saveSettings(configFile);
  }
  else
  {
    // Open the config file, get settings, set them to the application controller
    QSettings settings(m_configFilePath, QSettings::IniFormat);
    settings.beginGroup("Settings");
    // get the values from the config, and write to the settings map
    for (const QString& key : m_dsaSettings.keys())
    {
      if (settings.value(key).toStringList().length() > 1)
        m_dsaSettings[key] = settings.value(key).toStringList();
      else
        m_dsaSettings[key] = settings.value(key).toString();
    }
  }
}

// This creates the default values for the config file. If the app
// starts and there is no config file, it will create one, and write
// the following values to the file.
void DsaController::createDefaultSettings()
{
  // setup the defaults
  m_dsaSettings["RootDataDirectory"] = DsaUtility::dataPath();
  m_dsaSettings["BasemapDirectory"] = QString("%1/BasemapData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["ElevationDirectory"] = QString("%1/ElevationData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["SimulationDirectory"] = QString("%1/SimulationData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["ResourceDirectory"] = QString("%1/ResourceData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["LocalDataPaths"] = QStringList { QString("%1/OperationalData").arg(m_dsaSettings["RootDataDirectory"].toString()), m_dsaSettings["RootDataDirectory"].toString() };
  m_dsaSettings["DefaultBasemap"] = "topographic";
  m_dsaSettings["DefaultElevationSource"] = QString("%1/CaDEM.tpk").arg(m_dsaSettings["ElevationDirectory"].toString());
  m_dsaSettings["GpxFile"] = QString("%1/MontereyMounted.gpx").arg(m_dsaSettings["SimulationDirectory"].toString());
  m_dsaSettings["SimulateLocation"] = "true";
}

void DsaController::saveSettings(QFile& configFile)
{
  // create file
  if (configFile.open(QIODevice::ReadWrite))
  {
    QTextStream stream(&configFile);
    stream << "[Settings]\n";

    // write defaults to the config file
    for (const QString& key : m_dsaSettings.keys())
    {
      stream << key << "=" << m_dsaSettings.value(key).toStringList().join(",") << "\n";
    }
  }
}
