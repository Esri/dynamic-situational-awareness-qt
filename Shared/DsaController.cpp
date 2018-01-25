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

// API
#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "ElevationSource.h"
#include "GeoView.h"

// Toolkit
#include "AbstractTool.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// Dsa apps
#include "DsaUtility.h"
#include "DsaController.h"

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
  Toolkit::ToolResourceProvider::instance()->setScene(m_scene);
  Toolkit::ToolResourceProvider::instance()->setGeoView(geoView);

  for(Toolkit::AbstractTool* abstractTool : Toolkit::ToolManager::instance())
  {
    if (!abstractTool)
      continue;

    abstractTool->setProperties(m_dsaSettings);

    connect(abstractTool, &Toolkit::AbstractTool::errorOccurred, this, &DsaController::onError);
    connect(abstractTool, &Toolkit::AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);
  }
}

void DsaController::onError(const Error& e)
{
  qDebug() << "Error" << e.message() << e.additionalMessage();
  emit errorOccurred(e.message(), e.additionalMessage());
}

void DsaController::onPropertyChanged(const QString &propertyName, const QVariant &propertyValue)
{
  m_dsaSettings.insert(propertyName, propertyValue);
  // save the settings
  QFile configFile(m_configFilePath);
  saveSettings(configFile);
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
  m_dsaSettings["MessageFeedUdpPorts"] = QStringList { QString("45678"), QString("45679") };
  m_dsaSettings["MessageFeeds"] = QStringList { QString("Cursor-on-Target:cot:mil2525c"),
      QString("Friendly Tracks:position_report:mil2525c"), QString("Contact Reports:spotrep:enemycontact1600.png"),
      QString("Situation Reports:sitrep:sitrep1600.png"), QString("EOD Reports:eod:eod1600.png"),
      QString("Sensor Observations:sensor_obs:sensorobs1600.png") };
  m_dsaSettings["InitialLocation"] = QStringList { QString::number(DsaUtility::montereyCA().x()),
      QString::number(DsaUtility::montereyCA().y()), QString("5000.0"), QString("0.0"),
      QString("75.0"), QString("0.0") };
  m_dsaSettings["CoordinateFormat"] = QString("Degrees Minutes Seeconds");
  m_dsaSettings["UnitOfMeasurement"] = QString("Meters");
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
