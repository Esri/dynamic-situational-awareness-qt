
/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "DsaController.h"

// example app headers
#include "AlertConstants.h"
#include "AlertLevel.h"
#include "AppConstants.h"
#include "BasemapPickerController.h"
#include "ContextMenuController.h"
#include "DsaUtility.h"
#include "LayerCacheManager.h"
#include "MessageFeedConstants.h"
#include "OpenMobileScenePackageController.h"

// toolkit headers
#include "CoordinateConversionConstants.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "GeoView.h"
#include "Scene.h"

// Qt headers
#include <QDir>
#include <QFileInfo>
#include <QHostInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

namespace Dsa {

bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);

/*!
  \class Dsa::DsaController
  \inmodule Dsa
  \inherits QObject
  \brief This is the controller for the DSA app. It is responsible for connecting the
  view (such as the \l Esri::ArcGISRuntime::GeoView) to the business logic of the app.

  For example, signals from the view are passed to the \l Toolkit::ToolResourceProvider
  where they can be accessed by the list of \l Esri::ArcGISRuntime::Toolkit::AbstractTool objects stored in
  the \l Esri::ArcGISRuntime::Toolkit::ToolManager.

  This type is also responsible for reading and writing app configuration details to
  a JSON settings file. Information in the JSON file is sent to each tool as a set of
  properties.
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
DsaController::DsaController(QObject* parent):
  QObject(parent),
  m_scene(new Scene(this)),
  m_jsonFormat(QSettings::registerFormat("json", &readJsonFile, &writeJsonFile)),
  m_conflictingToolNames{QStringLiteral("Alert Conditions"),
                         QStringLiteral("Markup Tool"),
                         QStringLiteral("viewshed"),
                         QStringLiteral("Observation Report")}
{
  // setup config settings
  setupConfig();
  m_scene->setInitialViewpoint(defaultViewpoint());
  m_dataPath = m_dsaSettings["RootDataDirectory"].toString();

  connect(m_scene, &Scene::errorOccurred, this, &DsaController::onError);

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::sceneChanged, this, [this]()
  {
    m_scene = ToolResourceProvider::instance()->scene();
  });
}

/*!
  \brief Destructor.
 */
DsaController::~DsaController()
{
  // save the settings
  saveSettings();
}

/*!
  \brief Returns the Esri::ArcGISRuntime::Scene used by the app.
 */
Scene* DsaController::scene() const
{
  return m_scene;
}

/*!
  \brief Initialize the app with the Esri::ArcGISRuntime::GeoView \a geoView.

  When this method is called, the various tools in the app are initialized.
 */
void DsaController::init(GeoView* geoView)
{
  auto openScenePackageTool = Toolkit::ToolManager::instance().tool<OpenMobileScenePackageController>();
  if (openScenePackageTool)
    openScenePackageTool->setProperties(m_dsaSettings);

  m_cacheManager = new LayerCacheManager(this);

  if (openScenePackageTool)
    m_cacheManager->addExcludedPath(openScenePackageTool->packageDataPath());

  Toolkit::ToolResourceProvider::instance()->setScene(m_scene);
  Toolkit::ToolResourceProvider::instance()->setGeoView(geoView);

  // set the selection color for graphics and features
  geoView->setSelectionProperties(SelectionProperties(Qt::red));

  // connect all tool signals
  for(Toolkit::AbstractTool* abstractTool : Toolkit::ToolManager::instance())
  {
    if (!abstractTool)
      continue;

    abstractTool->setProperties(m_dsaSettings);

    connect(abstractTool, &Toolkit::AbstractTool::errorOccurred, this, &DsaController::onError);
    connect(abstractTool, &Toolkit::AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);

    if (abstractTool->metaObject()->indexOfSignal("toolErrorOccurred(QString,QString)") != -1)
      connect(abstractTool, SIGNAL(toolErrorOccurred(QString,QString)), this, SLOT(onToolError(QString, QString)));

    // certain tools can conflict - for example, those which interact directly with the view
    if (!isConflictingTool(abstractTool->toolName()))
      continue;

    // whenever a conflciting tool is activated, deactivate all of the other conflicting tools
    connect(abstractTool, &Toolkit::AbstractTool::activeChanged, this, [this, abstractTool]()
    {
      bool anyActive = false;

      // if this tool is becoming active, deactivate all conflicting tools
      if (abstractTool->isActive())
      {
        anyActive = true;
        auto toolsIt = Toolkit::ToolManager::instance().begin();
        auto toolsEnd = Toolkit::ToolManager::instance().end();
        for (; toolsIt != toolsEnd; ++toolsIt)
        {
          Toolkit::AbstractTool* candidateTool = *toolsIt;
          if (!candidateTool)
            continue;

          if (candidateTool->toolName() == abstractTool->toolName())
            continue;

          if (!isConflictingTool(candidateTool->toolName()))
            continue;

          if (candidateTool->isActive())
            candidateTool->setActive(false);
        }
      }
      // otherwise, if this tool is becoming deactivated, check if any of the conflicting tools are now active
      else
      {

        auto toolsIt = Toolkit::ToolManager::instance().begin();
        auto toolsEnd = Toolkit::ToolManager::instance().end();
        for (; toolsIt != toolsEnd; ++toolsIt)
        {
          Toolkit::AbstractTool* candidateTool = *toolsIt;
          if (!candidateTool)
            continue;

          if (!isConflictingTool(candidateTool->toolName()))
            continue;

          if (!candidateTool->isActive())
            continue;

          anyActive = true;
          break;
        }
      }

      // The context menu should only be active when the other tools which interact with the view are not
      ContextMenuController* contextMenu = Toolkit::ToolManager::instance().tool<ContextMenuController>();
      if (contextMenu && contextMenu->isActive() == anyActive)
        contextMenu->setActive(!anyActive);
    });
  }
}

/*!
 * \brief Slot to handle an ArcGISRuntime Error \a error.
 */
void DsaController::onError(const Error& error)
{
  qDebug() << "Error" << error.message() << error.additionalMessage();
  emit errorOccurred(error.message(), error.additionalMessage());
}

/*!
 * \brief Slot to handle an \a errorMessage (with an \a additionalMessage) from an \l AbstractTool.
 */
void DsaController::onToolError(const QString& errorMessage, const QString& additionalMessage)
{
  qDebug() << "Error" << errorMessage << additionalMessage;
  emit errorOccurred(errorMessage, additionalMessage);
}

/*!
 * \brief Slot to handle a change to the \a propertyName to the new value \l propertyValue.
 */
void DsaController::onPropertyChanged(const QString& propertyName, const QVariant& propertyValue)
{
  if (m_dsaSettings.value(propertyName) == propertyValue)
    return;

  m_dsaSettings.insert(propertyName, propertyValue);
  // save the settings
  saveSettings();

  // inform tools of the change
  auto it = Toolkit::ToolManager::instance().begin();
  auto itEnd = Toolkit::ToolManager::instance().end();
  for (;it != itEnd; ++it)
  {
    Toolkit::AbstractTool* tool = *it;
    if (!tool)
      continue;

    disconnect(tool, &Toolkit::AbstractTool::propertyChanged,this, &DsaController::onPropertyChanged);
    tool->setProperties(m_dsaSettings);
    connect(tool, &Toolkit::AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);
  }

}

/*!
 * \internal
 */
Viewpoint DsaController::initialLocationFromConfig()
{
  // Attempt to set the scene's initialViewpoint from JSON configuration
  const auto findIt = m_dsaSettings.constFind(QStringLiteral("InitialLocation"));

  // If no initial location is specified, default to Monterey, CA
  if (findIt == m_dsaSettings.constEnd())
    return Viewpoint();

  const QVariant initialLocVar = findIt.value();
  if (initialLocVar.isNull())
    return Viewpoint();

  const QJsonObject initialLocation = QJsonObject::fromVariantMap(initialLocVar.toMap());
  if (initialLocation.isEmpty())
    return Viewpoint();

  // set the initial center Point from JSON if it is found
  auto centerIt = initialLocation.find("center");
  if (centerIt == initialLocation.constEnd())
    return Viewpoint();

  const QJsonValue centerVal = centerIt.value();
  const QJsonDocument centerDoc = QJsonDocument(centerVal.toObject());
  const auto newCenter = Point::fromJson(centerDoc.toJson(QJsonDocument::JsonFormat::Compact));

  // set the initial distance from JSON if it is found (if not default to 5000)
  auto distanceIt = initialLocation.find("distance");
  double initialDistance = distanceIt != initialLocation.constEnd() ? distanceIt.value().toDouble(5000.0)
                                                                    : 5000.0;

  // set the initial heading from JSON if it is found (if not default to 0.0)
  auto headingIt = initialLocation.find("heading");
  double initialHeading = headingIt != initialLocation.constEnd() ? headingIt.value().toDouble(0.0)
                                                                  : 0.0;

  // set the initial pitch from JSON if it is found (if not default to 0.0)
  auto pitchIt = initialLocation.find("pitch");
  double initialPitch = pitchIt != initialLocation.constEnd() ? pitchIt.value().toDouble(0.0)
                                                              : 0.0;

  // set the initial roll from JSON if it is found (if not default to 0.0)
  auto rollIt = initialLocation.find("roll");
  double initialRoll = rollIt != initialLocation.constEnd() ? rollIt.value().toDouble(0.0)
                                                            : 0.0;

  // Return the initial viewpoint
  const Camera initCamera(newCenter, initialDistance, initialHeading, initialPitch, initialRoll);
  Viewpoint initViewpoint(newCenter, initCamera);
  return initViewpoint;
}

/*!
 * \internal
 */
Viewpoint DsaController::defaultViewpoint()
{
  // If there is no configuration setting, default the initial viewpoint to Monterey, CA
  // NOTE that if using a MobileScenePackage, this will be replaced with the
  // selected scene's initialViewpoint
  Viewpoint initLocationFromConfig = initialLocationFromConfig();
  if (initLocationFromConfig.isEmpty())
  {
    const Camera initCamera(DsaUtility::montereyCA(), 5000.0, 0.0, 75.0, 0.0);
    Viewpoint initViewpoint(DsaUtility::montereyCA(), initCamera);
    return initViewpoint;
  }
  else
  {
    return initLocationFromConfig;
  }
}

/*!
  \brief Creates a new scene with the default basemap and default surface.
 */
void DsaController::resetToDefaultScene()
{
  // obtain other required tools
  auto basemapTool = Toolkit::ToolManager::instance().tool<BasemapPickerController>();
  if (!basemapTool)
    return;

  // create scene
  Scene* newScene = new Scene(this);
  newScene->setInitialViewpoint(defaultViewpoint());

  // set on sceneview
  Toolkit::ToolResourceProvider::instance()->setScene(newScene);

  // add basemap
  basemapTool->selectInitialBasemap();

  // add elevation
  m_cacheManager->addElevation(m_dsaSettings);

  // remove any layers if automatically added
  if (!newScene->operationalLayers()->isEmpty())
  {
    for (auto layer : *newScene->operationalLayers())
    {
      delete layer;
      layer = nullptr;
    }
    newScene->operationalLayers()->clear();
  }

  // clear current scene and index in properties
  onPropertyChanged(AppConstants::SCENEINDEX_PROPERTYNAME, -1);
  onPropertyChanged(AppConstants::CURRENTSCENE_PROPERTYNAME, "");
  onPropertyChanged(AppConstants::LAYERS_PROPERTYNAME, QJsonArray().toVariantList());
  m_cacheManager->setProperties(m_dsaSettings);
}

/*!
 * \internal
 */
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

  // Add a condition "Distress" when an object from the Friendly Tracks Land feed has attribute status911 = 1
  QJsonObject conditionJson;
  conditionJson.insert(AlertConstants::CONDITION_NAME, QStringLiteral("Distress"));
  conditionJson.insert(AlertConstants::CONDITION_LEVEL, static_cast<int>(AlertLevel::Critical));
  conditionJson.insert(AlertConstants::CONDITION_TYPE, AlertConstants::attributeEqualsAlertConditionType());
  conditionJson.insert(AlertConstants::CONDITION_SOURCE, QStringLiteral("Friendly Tracks - Land"));
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

  QJsonArray messageFeedsJson;

  QJsonObject cotMessageFeedJson;
  cotMessageFeedJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("SA Events"));
  cotMessageFeedJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("cot"));
  cotMessageFeedJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("mil2525c"));
  cotMessageFeedJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("saevents.png"));
  cotMessageFeedJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("draped"));
  messageFeedsJson.append(cotMessageFeedJson);

  QJsonObject friendlyTracksLandJson;
  friendlyTracksLandJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("Friendly Tracks - Land"));
  friendlyTracksLandJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("position_report_land"));
  friendlyTracksLandJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("mil2525c"));
  friendlyTracksLandJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("friendlytracks.png"));
  friendlyTracksLandJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("draped"));
  messageFeedsJson.append(friendlyTracksLandJson);

  QJsonObject friendlyTracksAirJson;
  friendlyTracksAirJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("Friendly Tracks - Air"));
  friendlyTracksAirJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("position_report_air"));
  friendlyTracksAirJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("mil2525c"));
  friendlyTracksAirJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("friendlytracks-air.png"));
  friendlyTracksAirJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("absolute"));
  messageFeedsJson.append(friendlyTracksAirJson);

  QJsonObject spotRepJson;
  spotRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("Observation Reports"));
  spotRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("spotrep"));
  spotRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("observation1600.png"));
  spotRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("observation1600.png"));
  spotRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("draped"));
  messageFeedsJson.append(spotRepJson);

  QJsonObject sitRepJson;
  sitRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("Situation Reports"));
  sitRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("sitrep"));
  sitRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("sitrep1600.png"));
  sitRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("sitrep1600.png"));
  sitRepJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("draped"));
  messageFeedsJson.append(sitRepJson);

  QJsonObject eodReportsJson;
  eodReportsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("EOD Reports"));
  eodReportsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("eod"));
  eodReportsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("eod1600.png"));
  eodReportsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("eod1600.png"));
  eodReportsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("draped"));
  messageFeedsJson.append(eodReportsJson);

  QJsonObject sensorObsJson;
  sensorObsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_NAME, QStringLiteral("Sensor Observations"));
  sensorObsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_TYPE, QStringLiteral("sensor_obs"));
  sensorObsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("sensorobs1600.png"));
  sensorObsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_RENDERER, QStringLiteral("sensorobs1600.png"));
  sensorObsJson.insert(MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT, QStringLiteral("draped"));
  messageFeedsJson.append(sensorObsJson);
  m_dsaSettings[MessageFeedConstants::MESSAGE_FEEDS_PROPERTYNAME] = messageFeedsJson;

  QJsonObject locationBroadcastJson;
  locationBroadcastJson.insert(MessageFeedConstants::LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE, QStringLiteral("position_report_land"));
  locationBroadcastJson.insert(MessageFeedConstants::LOCATION_BROADCAST_CONFIG_PORT, 45679);
  m_dsaSettings[MessageFeedConstants::LOCATION_BROADCAST_CONFIG_PROPERTYNAME] = locationBroadcastJson;

  QJsonObject observationReportJson;
  observationReportJson.insert(MessageFeedConstants::OBSERVATION_REPORT_CONFIG_PORT, 45679);
  m_dsaSettings[MessageFeedConstants::OBSERVATION_REPORT_CONFIG_PROPERTYNAME] = observationReportJson;
}

/*! \brief internal
 *
 * Returns \c true if this tool is considered to conlfict with other tools.
 *
 * For example, this could be tools which interact directly with the view.
 */
bool DsaController::isConflictingTool(const QString& toolName) const
{
  return m_conflictingToolNames.contains(toolName);
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
  m_dsaSettings[AppConstants::USERNAME_PROPERTYNAME] = QHostInfo::localHostName();
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
  m_dsaSettings[Toolkit::CoordinateConversionConstants::COORDINATE_FORMAT_PROPERTY] = Toolkit::CoordinateConversionConstants::MGRS_FORMAT;
  m_dsaSettings[AppConstants::UNIT_OF_MEASUREMENT_PROPERTYNAME] = AppConstants::UNIT_METERS;
  m_dsaSettings["UseGpsForElevation"] = QStringLiteral("true");
  QJsonObject markupJson;
  markupJson.insert(QStringLiteral("port"), 12345);
  m_dsaSettings[QStringLiteral("MarkupConfig")] = markupJson;
  writeDefaultConditions();
  m_dsaSettings[OpenMobileScenePackageController::PACKAGE_DIRECTORY_PROPERTYNAME] = QString("%1/Packages").arg(m_dsaSettings["RootDataDirectory"].toString());
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

} // Dsa

// Signal Documentation

/*!
  \fn void DsaController::errorOccurred(const QString& message, const QString& additionalMessage);

  \brief Signal emitted when an error occurs.

  An error \a message and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */
