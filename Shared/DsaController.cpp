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
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

using namespace Esri::ArcGISRuntime;

bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);

DsaController::DsaController(QObject* parent):
  QObject(parent),
  m_scene(new Scene(this)),
  m_jsonFormat(QSettings::registerFormat("json", &readJsonFile, &writeJsonFile))
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
  saveSettings();
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

void DsaController::onPropertyChanged(const QString& propertyName, const QVariant& propertyValue)
{
  m_dsaSettings.insert(propertyName, propertyValue);
  // save the settings
  saveSettings();
}

void DsaController::setupConfig()
{
  // create the default settings map
  createDefaultSettings();

  // get the app config
  m_configFilePath = QString("%1/%2").arg(m_dsaSettings["RootDataDirectory"].toString(), "DsaAppConfig.json");

  // If the config file does not exist, create it, and set all of the defaults
  if (!QFileInfo::exists(m_configFilePath))
  {
    saveSettings();
  }
  else
  {
    // Open the config file, get settings, set them to the application controller
    QSettings settings(m_configFilePath, m_jsonFormat);
    const QStringList allKeys = settings.allKeys();
    // get the values from the config, and write to the settings map
    for (const QString& key : allKeys)
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
m_dsaSettings["DefaultBasemap"] = QStringLiteral("topographic");
m_dsaSettings["DefaultElevationSource"] = QString("%1/CaDEM.tpk").arg(m_dsaSettings["ElevationDirectory"].toString());
m_dsaSettings["GpxFile"] = QString("%1/MontereyMounted.gpx").arg(m_dsaSettings["SimulationDirectory"].toString());
m_dsaSettings["SimulateLocation"] = QStringLiteral("true");
m_dsaSettings["MessageFeedUdpPorts"] = QStringList { QString("45678"), QString("45679") };
m_dsaSettings["MessageFeeds"] = QStringList { QString("Cursor-on-Target:cot:mil2525c"),
    QString("Friendly Tracks:position_report:mil2525c"), QString("Contact Reports:spotrep:enemycontact1600.png"),
    QString("Situation Reports:sitrep:sitrep1600.png"), QString("EOD Reports:eod:eod1600.png"),
    QString("Sensor Observations:sensor_obs:sensorobs1600.png") };
m_dsaSettings["LocationBroadcastConfig"] = QStringList { QString("position_report"), QString("45679") };
m_dsaSettings["InitialLocation"] = QStringList { QString::number(DsaUtility::montereyCA().x()),
    QString::number(DsaUtility::montereyCA().y()), QString("5000.0"), QString("0.0"),
    QString("75.0"), QString("0.0") };
m_dsaSettings["CoordinateFormat"] = QStringLiteral("DMS");
m_dsaSettings["UnitOfMeasurement"] = QStringLiteral("meters");
m_dsaSettings["UseGpsForElevation"] = QStringLiteral("true");
}

void DsaController::saveSettings()
{
  QSettings settings(m_configFilePath, m_jsonFormat);

  auto it = m_dsaSettings.cbegin();
  auto itEnd = m_dsaSettings.cend();
  for (; it != itEnd; ++it)
  {
    const QVariant& val = it.value();
    QString stringVal = val.toStringList().join(",");
    if (stringVal.isEmpty())
      stringVal = val.toString();

    settings.setValue(it.key(), stringVal);
  }
}

bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map)
{
  const QByteArray data = device.readAll();
  const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

  const QJsonObject jsonObject = jsonDoc.object();

  map = jsonObject.toVariantMap();

  return !map.isEmpty();
}

bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map)
{
  QJsonObject jsonObject = QJsonObject::fromVariantMap(map);

  if (jsonObject.isEmpty())
    return false;

  QJsonDocument conditionsJsonDoc(jsonObject);
  const qint64 writtenBytes = device.write(conditionsJsonDoc.toJson(QJsonDocument::Indented));

  return writtenBytes != -1;
}
