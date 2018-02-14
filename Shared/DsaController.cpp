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
#include "FeatureLayer.h"
#include "RasterLayer.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "FeatureTable.h"
#include "GeoPackage.h"
#include "GeoPackageFeatureTable.h"
#include "GeoPackageRaster.h"
#include "ShapefileFeatureTable.h"
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledLayer.h"
#include "ArcGISVectorTiledLayer.h"
#include "Raster.h"

// Toolkit
#include "AbstractTool.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "LayerCacheManager.h"

// Dsa apps
#include "AlertLevel.h"
#include "AlertConstants.h"
#include "DsaUtility.h"
#include "DsaController.h"
#include "MessageFeedConstants.h"

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
  m_jsonFormat(QSettings::registerFormat("json", &readJsonFile, &writeJsonFile)),
  m_conflictingToolNames{QStringLiteral("Alert Conditions"),
                         QStringLiteral("identify"),
                         QStringLiteral("Markup Tool"),
                         QStringLiteral("CoordinateConversion"),
                         QStringLiteral("analysis")}
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

  m_cacheManager = new LayerCacheManager(this);

  for(Toolkit::AbstractTool* abstractTool : Toolkit::ToolManager::instance())
  {
    if (!abstractTool)
      continue;

    abstractTool->setProperties(m_dsaSettings);

    connect(abstractTool, &Toolkit::AbstractTool::errorOccurred, this, &DsaController::onError);
    connect(abstractTool, &Toolkit::AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);

    if (!m_conflictingToolNames.contains(abstractTool->toolName()))
      continue;

    connect(abstractTool, &Toolkit::AbstractTool::activeChanged, this, [this, abstractTool]()
    {
      if (!abstractTool->isActive())
        return;

      auto toolsIt = Toolkit::ToolManager::instance().begin();
      auto toolsEnd = Toolkit::ToolManager::instance().end();
      for (; toolsIt != toolsEnd; ++toolsIt)
      {
        Toolkit::AbstractTool* candidateTool = *toolsIt;
        if (!candidateTool)
          continue;

        if (candidateTool->toolName() == abstractTool->toolName())
          continue;

        if (!m_conflictingToolNames.contains(candidateTool->toolName()))
          continue;

        candidateTool->setActive(false);
      }
    });
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
  m_configFilePath = QString("%1/%2").arg(m_dsaSettings["RootDataDirectory"].toString(), QStringLiteral("DsaAppConfig.json"));

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
      m_dsaSettings[key] = settings.value(key);
  }
}

/*! \brief internal
 *
 * Writes the default initial location (in this app Monterey California)
 * as JSON to the settings map.
 */
void DsaController::writeDefaultInitialLocation()
{
  QJsonObject initialLocationJson;
  const QString centerString = DsaUtility::montereyCA().toJson();
  const QJsonDocument centerDoc = QJsonDocument::fromJson(centerString.toLatin1());
  initialLocationJson.insert( QStringLiteral("center"), centerDoc.object());
  initialLocationJson.insert( QStringLiteral("distance"), 5000.0);
  initialLocationJson.insert( QStringLiteral("heading"), 0.0);
  initialLocationJson.insert( QStringLiteral("pitch"), 75.0);
  initialLocationJson.insert( QStringLiteral("roll"), 0.0);
  m_dsaSettings[QStringLiteral("InitialLocation")] = initialLocationJson.toVariantMap();
}

/*! \brief internal
 *
 * Writes the default local data paths as JSON to the settings map.
 */
void DsaController::writeDefaultLocalDataPaths()
{
  const QString rootDir = m_dsaSettings["RootDataDirectory"].toString();
  QStringList pathsList{QString("%1/").arg(rootDir),
        QString("%1/OperationalData").arg(rootDir)};
  m_dsaSettings[QStringLiteral("LocalDataPaths")] = pathsList;
}

/*! \brief internal
 *
 * Writes the default Alert Conditions as JSON to the settings map.
 */
void DsaController::writeDefaultConditions()
{
  QJsonArray allConditionsJson;

  // Add a condition "Distress" when an object from the Friendly Tracks feed has attribute status911 = 1
  QJsonObject conditionJson;
  conditionJson.insert(AlertConstants::CONDITION_NAME, QStringLiteral("Distress"));
  conditionJson.insert(AlertConstants::CONDITION_LEVEL, static_cast<int>(AlertLevel::Critical));
  conditionJson.insert(AlertConstants::CONDITION_TYPE, AlertConstants::attributeEqualsAlertConditionType());
  conditionJson.insert(AlertConstants::CONDITION_SOURCE, QStringLiteral("Friendly Tracks"));
  QJsonObject queryObject;
  queryObject.insert(AlertConstants::ATTRIBUTE_NAME, QStringLiteral("status911"));
  conditionJson.insert(AlertConstants::CONDITION_QUERY, queryObject);
  conditionJson.insert(AlertConstants::CONDITION_TARGET, "1");
  allConditionsJson.append(conditionJson);
  m_dsaSettings.insert(AlertConstants::ALERT_CONDITIONS_PROPERTYNAME, allConditionsJson.toVariantList());
}

/*! \brief internal
 *
 * Writes the default message feeds to the settings map.
 */
void DsaController::writeDefaultMessageFeeds()
{
  m_dsaSettings[MessageFeedConstants::MESSAGE_FEED_UDP_PORTS_PROPERTYNAME] = QStringList { QString("45678"), QString("45679") };
  m_dsaSettings[MessageFeedConstants::MESSAGE_FEEDS_PROPERTYNAME] = QStringList { QString("Cursor-on-Target:cot:mil2525c"),
      QString("Friendly Tracks:position_report:mil2525c"), QString("Contact Reports:spotrep:enemycontact1600.png"),
      QString("Situation Reports:sitrep:sitrep1600.png"), QString("EOD Reports:eod:eod1600.png"),
      QString("Sensor Observations:sensor_obs:sensorobs1600.png") };
  m_dsaSettings[MessageFeedConstants::LOCATION_BROADCAST_CONFIG_PROPERTYNAME] = QStringList { QString("position_report"), QString("45679") };
}

/*! \brief internal
 *
 * This creates the default values for the config file. If the app
 * starts and there is no config file, it will create one, and write
 * the following values to the file.
 */
void DsaController::createDefaultSettings()
{
  // setup the defaults
  m_dsaSettings["RootDataDirectory"] = DsaUtility::dataPath();
  m_dsaSettings["BasemapDirectory"] = QString("%1/BasemapData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["ElevationDirectory"] = QString("%1/ElevationData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["SimulationDirectory"] = QString("%1/SimulationData").arg(m_dsaSettings["RootDataDirectory"].toString());
  m_dsaSettings["ResourceDirectory"] = QString("%1/ResourceData").arg(m_dsaSettings["RootDataDirectory"].toString());
  writeDefaultLocalDataPaths();
  m_dsaSettings["DefaultBasemap"] = QStringLiteral("topographic");
  m_dsaSettings["DefaultElevationSource"] = QString("%1/CaDEM.tpk").arg(m_dsaSettings["ElevationDirectory"].toString());
  m_dsaSettings["GpxFile"] = QString("%1/MontereyMounted.gpx").arg(m_dsaSettings["SimulationDirectory"].toString());
  m_dsaSettings["SimulateLocation"] = QStringLiteral("true");
  writeDefaultMessageFeeds();
  writeDefaultInitialLocation();
  m_dsaSettings["CoordinateFormat"] = QStringLiteral("DMS");
  m_dsaSettings["UnitOfMeasurement"] = QStringLiteral("meters");
  m_dsaSettings["UseGpsForElevation"] = QStringLiteral("true");
  writeDefaultConditions();
}

/*!
 * \brief Save the app properties to a custom JSON QSettings file.
 */
void DsaController::saveSettings()
{
  QSettings settings(m_configFilePath, m_jsonFormat);

  auto it = m_dsaSettings.cbegin();
  auto itEnd = m_dsaSettings.cend();
  for (; it != itEnd; ++it)
    settings.setValue(it.key(), it.value());
}

/*! \brief Read method for custom QSettings JSON format
 *
 * Attempts to read the information in \a device in a JSON format
 * and stores the results in \a map.
 *
 * Returns \c true if the read was successful and \c false if not.
 */
bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map)
{
  const QByteArray data = device.readAll();
  const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

  const QJsonObject jsonObject = jsonDoc.object();

  map = jsonObject.toVariantMap();

  return !map.isEmpty();
}

/*! \brief Write method for custom QSettings JSON format
 *
 * Attempts to encode the information in \a map as valid JSON
 * and writes the results to \a device.
 *
 * Returns \c true if the write was successful and \c false if not.
 */
bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map)
{
  const QJsonObject jsonObject = QJsonObject::fromVariantMap(map);

  if (jsonObject.isEmpty())
    return false;

  const QJsonDocument conditionsJsonDoc(jsonObject);
  const qint64 writtenBytes = device.write(conditionsJsonDoc.toJson(QJsonDocument::Indented));

  return writtenBytes != -1;
}

