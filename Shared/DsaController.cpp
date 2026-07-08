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

// C++ API headers
#include "Camera.h"
#include "Error.h"
#include "GeoView.h"
#include "Layer.h"
#include "LayerListModel.h"
#include "Scene.h"
#include "SelectionProperties.h"
#include "Viewpoint.h"

// Toolkit headers
#include "CoordinateConversionConstants.h"

// Qt headers
#include <QDir>
#include <QFileInfo>
#include <QFuture>
#include <QHostInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

// DSA headers
#include "AlertConstants.h"
#include "AppConstants.h"
#include "BasemapPickerController.h"
#include "ContextMenuController.h"
#include "DsaUtility.h"
#include "GridController.h"
#include "LayerCacheManager.h"
#include "LocationController.h"
#include "MessageFeedConstants.h"
#include "OpenMobileScenePackageController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

namespace
{

bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
Viewpoint viewpointFromJson(const QJsonObject& initialLocation);
QJsonObject defaultViewpoint();

} // namespace

/*!
  \class Dsa::DsaController
  \inmodule Dsa
  \inherits QObject
  \brief This is the controller for the DSA app. It is responsible for connecting the
  view (such as the \l Esri::ArcGISRuntime::GeoView) to the business logic of the app.

  For example, signals from the view are passed to the \l ToolResourceProvider
  where they can be accessed by the list of \l AbstractTool objects stored in
  the \l Esri::ArcGISRuntime::ToolManager.

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
  // set the current path for the application to the active configuration directory
  // so we can use relative paths for data in the configuration file
  QDir::setCurrent(DsaUtility::activeConfigurationPath());

  // setup config settings
  setupConfig();
  m_scene->setInitialViewpoint(viewpointFromJson(defaultViewpoint()));
  m_dataPath = m_dsaSettings[AppConstants::PROPERTYNAME_ROOT_DATA_DIRECTORY].toString();

  connect(m_scene, &Scene::errorOccurred, this, &DsaController::onError);

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::sceneChanged, this, [this, firstLoad{true}]() mutable
  {
    m_scene = ToolResourceProvider::instance()->scene();

    // Here we set up the initial location of the scene fron the config. We do this on the first scene
    // we load when the application starts up. On any subsequent scenes that the user loads
    // via the UI, we take this new initial location as the source of truth from then on and
    // write it to file.
    if (!m_scene)
      return;

    updateInitialLocationOnSceneChange(firstLoad);
    firstLoad = false;
  });

  Q_ASSERT(s_instance == nullptr); // there should never be more than one DsaController created
  s_instance = this;
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
  ToolResourceProvider::instance()->setGeoView(geoView);

  bool hasActiveScene = false;
  auto openScenePackageTool = ToolManager::instance().tool<OpenMobileScenePackageController>();
  if (openScenePackageTool)
  {
    openScenePackageTool->toolInitProperties(m_dsaSettings);
    hasActiveScene = openScenePackageTool->hasActiveScene();
  }

  m_cacheManager = new LayerCacheManager(this);
  if (openScenePackageTool && !openScenePackageTool->packageDataPath().isEmpty())
    m_cacheManager->addExcludedPath(openScenePackageTool->packageDataPath());

  // Only set the default scene if the scene package tool hasn't set a scene.
  if (!hasActiveScene)
  {
    if (!m_dsaSettings.contains(AppConstants::PROPERTYNAME_INITIAL_LOCATION))
    {
      // While a scene change would normally write out the viewpoint
      // to config if/when it is missing, this here is a special case. We
      // want the config file to contain a nice human-editable initial location
      // using a distance measure. Extracting the viewpoint from the scene
      // will give us a calculated viewpoint with no distance measure, so we have
      // to set this manually.
      m_dsaSettings[AppConstants::PROPERTYNAME_INITIAL_LOCATION] = defaultViewpoint();
    }

    ToolResourceProvider::instance()->setScene(m_scene);
  }
  // set the selection color for graphics and features
  geoView->setSelectionProperties(SelectionProperties(Qt::red));

  // connect all tool signals
  for(AbstractTool* abstractTool : ToolManager::instance())
  {
    if (!abstractTool)
      continue;

    abstractTool->toolInitProperties(m_dsaSettings);

    connect(abstractTool, &AbstractTool::errorOccurred, this, &DsaController::onError);
    connect(abstractTool, &AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);

    if (abstractTool->metaObject()->indexOfSignal("toolErrorOccurred(QString,QString)") != -1)
      connect(abstractTool, SIGNAL(toolErrorOccurred(QString,QString)), this, SLOT(onToolError(QString, QString)));

    // certain tools can conflict - for example, those which interact directly with the view
    if (!isConflictingTool(abstractTool->toolName()))
      continue;

    // whenever a conflciting tool is activated, deactivate all of the other conflicting tools
    connect(abstractTool, &AbstractTool::activeChanged, this, [this, abstractTool]()
    {
      bool anyActive = false;

      // if this tool is becoming active, deactivate all conflicting tools
      if (abstractTool->isActive())
      {
        anyActive = true;
        auto toolsIt = ToolManager::instance().begin();
        auto toolsEnd = ToolManager::instance().end();
        for (; toolsIt != toolsEnd; ++toolsIt)
        {
          AbstractTool* candidateTool = *toolsIt;
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

        auto toolsIt = ToolManager::instance().begin();
        auto toolsEnd = ToolManager::instance().end();
        for (; toolsIt != toolsEnd; ++toolsIt)
        {
          AbstractTool* candidateTool = *toolsIt;
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
      ContextMenuController* contextMenu = ToolManager::instance().tool<ContextMenuController>();
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
  for (auto* tool : ToolManager::instance())
  {
    if (!tool)
      continue;

    if (tool->shouldSetProperties(propertyName))
    {
      disconnect(tool, &AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);
      tool->toolInitProperties(m_dsaSettings);
      connect(tool, &AbstractTool::propertyChanged, this, &DsaController::onPropertyChanged);
    }
  }
}

/*!
  \brief Creates a new scene with the default basemap and default surface.
 */
void DsaController::resetToDefaultScene()
{
  // obtain other required tools
  auto basemapTool = ToolManager::instance().tool<BasemapPickerController>();
  if (!basemapTool)
    return;

  // create scene
  Scene* newScene = new Scene(this);
  if (const auto initialViewpoint = readInitialLocation(); !initialViewpoint.isEmpty())
    newScene->setInitialViewpoint(initialViewpoint);

  // set on sceneview
  ToolResourceProvider::instance()->setScene(newScene);

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
    }
    newScene->operationalLayers()->clear();
  }

  // clear current scene and index in properties
  onPropertyChanged(AppConstants::PROPERTYNAME_SCENE_INDEX, -1);
  onPropertyChanged(AppConstants::PROPERTYNAME_CURRENT_SCENE, "");
  onPropertyChanged(AppConstants::PROPERTYNAME_LAYERS, QJsonArray().toVariantList());
  m_cacheManager->toolInitProperties(m_dsaSettings);
}

const DsaController* DsaController::instance()
{
  return s_instance;
}

/*!
 * \internal
 */
void DsaController::setupConfig()
{
  // create the default settings map
  createDefaultSettings();

  // get the app config
  m_configFilePath = QString("%1/%2").arg(m_dsaSettings[AppConstants::PROPERTYNAME_ROOT_DATA_DIRECTORY].toString(), DsaUtility::FILE_NAME_APP_CONFIG);

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
  using namespace Dsa::MessageFeedConstants;
  m_dsaSettings[MESSAGE_FEED_UDP_PORTS_PROPERTYNAME] = QVariantList{ 45678, 45679 };

  QJsonArray messageFeedsJson;

  // create the default track display properties
  const std::vector<std::tuple<QString, QVariant>> trackDisplayDefaults{
    { MESSAGE_FEEDS_OBSERVATIONS_SHOW, false },
    { MESSAGE_FEEDS_OBSERVATIONS_COLOR, MESSAGE_FEEDS_TRACK_DISPLAY_COLOR_DEFAULT },
    { MESSAGE_FEEDS_OBSERVATIONS_SIZE, 10 },
    { MESSAGE_FEEDS_OBSERVATIONS_MAXIMUM, 5 },
    { MESSAGE_FEEDS_TRACK_LINE_SHOW, false },
    { MESSAGE_FEEDS_TRACK_LINE_COLOR, MESSAGE_FEEDS_TRACK_DISPLAY_COLOR_DEFAULT },
    { MESSAGE_FEEDS_TRACK_LINE_SIZE, 4 },
  };
  const auto itBeg = std::cbegin(trackDisplayDefaults);
  const auto itEnd = std::cend(trackDisplayDefaults);
  const auto appendAndAddTrackDisplayDefaults = [&](QJsonObject& o)
  {
    std::for_each(itBeg, itEnd, [&](const std::tuple<QString, QVariant>& t)
    {
      o.insert(std::get<0>(t), QJsonValue::fromVariant(std::get<1>(t)));
    });
    messageFeedsJson.append(o);
  };

  QJsonObject cotMessageFeedJson;
  cotMessageFeedJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("SA Events"));
  cotMessageFeedJson.insert(MESSAGE_FEEDS_TYPE, MessageFeeds::Types::CURSOR_ON_TARGET);
  cotMessageFeedJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("mil2525c"));
  cotMessageFeedJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("saevents.png"));
  cotMessageFeedJson.insert(MESSAGE_FEEDS_PLACEMENT, MESSAGE_FEEDS_PLACEMENT_DEFAULT);
  appendAndAddTrackDisplayDefaults(cotMessageFeedJson);

  QJsonObject friendlyTracksLandJson;
  friendlyTracksLandJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("Friendly Tracks - Land"));
  friendlyTracksLandJson.insert(MESSAGE_FEEDS_TYPE, QString{"%1_land"}.arg(MessageFeeds::Types::POSITION_REPORT));
  friendlyTracksLandJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("mil2525c"));
  friendlyTracksLandJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("friendlytracks.png"));
  friendlyTracksLandJson.insert(MESSAGE_FEEDS_PLACEMENT, MESSAGE_FEEDS_PLACEMENT_DEFAULT);
  appendAndAddTrackDisplayDefaults(friendlyTracksLandJson);

  QJsonObject friendlyTracksAirJson;
  friendlyTracksAirJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("Friendly Tracks - Air"));
  friendlyTracksAirJson.insert(MESSAGE_FEEDS_TYPE, QString{"%1_air"}.arg(MessageFeeds::Types::POSITION_REPORT));
  friendlyTracksAirJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("mil2525c"));
  friendlyTracksAirJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("friendlytracks-air.png"));
  friendlyTracksAirJson.insert(MESSAGE_FEEDS_PLACEMENT, QStringLiteral("absolute"));
  appendAndAddTrackDisplayDefaults(friendlyTracksAirJson);

  QJsonObject spotRepJson;
  spotRepJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("Observation Reports"));
  spotRepJson.insert(MESSAGE_FEEDS_TYPE, MessageFeeds::Types::SPOTREP);
  spotRepJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("observation1600.png"));
  spotRepJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("observation1600.png"));
  spotRepJson.insert(MESSAGE_FEEDS_PLACEMENT, MESSAGE_FEEDS_PLACEMENT_DEFAULT);
  appendAndAddTrackDisplayDefaults(spotRepJson);

  QJsonObject sitRepJson;
  sitRepJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("Situation Reports"));
  sitRepJson.insert(MESSAGE_FEEDS_TYPE, MessageFeeds::Types::SITREP);
  sitRepJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("sitrep1600.png"));
  sitRepJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("sitrep1600.png"));
  sitRepJson.insert(MESSAGE_FEEDS_PLACEMENT, MESSAGE_FEEDS_PLACEMENT_DEFAULT);
  appendAndAddTrackDisplayDefaults(sitRepJson);

  QJsonObject eodReportsJson;
  eodReportsJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("EOD Reports"));
  eodReportsJson.insert(MESSAGE_FEEDS_TYPE, MessageFeeds::Types::EOD);
  eodReportsJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("eod1600.png"));
  eodReportsJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("eod1600.png"));
  eodReportsJson.insert(MESSAGE_FEEDS_PLACEMENT, MESSAGE_FEEDS_PLACEMENT_DEFAULT);
  appendAndAddTrackDisplayDefaults(eodReportsJson);

  QJsonObject sensorObsJson;
  sensorObsJson.insert(MESSAGE_FEEDS_NAME, QStringLiteral("Sensor Observations"));
  sensorObsJson.insert(MESSAGE_FEEDS_TYPE, MessageFeeds::Types::SENSOR_OBS);
  sensorObsJson.insert(MESSAGE_FEEDS_RENDERER, QStringLiteral("sensorobs1600.png"));
  sensorObsJson.insert(MESSAGE_FEEDS_THUMBNAIL, QStringLiteral("sensorobs1600.png"));
  sensorObsJson.insert(MESSAGE_FEEDS_PLACEMENT, MESSAGE_FEEDS_PLACEMENT_DEFAULT);
  appendAndAddTrackDisplayDefaults(sensorObsJson);
  m_dsaSettings[MESSAGE_FEEDS_PROPERTYNAME] = messageFeedsJson;

  QJsonObject locationBroadcastJson;
  locationBroadcastJson.insert(LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE, QString{"%1_land"}.arg(MessageFeeds::Types::POSITION_REPORT));
  locationBroadcastJson.insert(LOCATION_BROADCAST_CONFIG_PORT, 45679);
  m_dsaSettings[LOCATION_BROADCAST_CONFIG_PROPERTYNAME] = locationBroadcastJson;

  QJsonObject observationReportJson;
  observationReportJson.insert(OBSERVATION_REPORT_CONFIG_PORT, 45679);
  m_dsaSettings[OBSERVATION_REPORT_CONFIG_PROPERTYNAME] = observationReportJson;
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
  m_dsaSettings[AppConstants::PROPERTYNAME_ROOT_DATA_DIRECTORY] = DsaUtility::FILE_PATH_CURRENT_DIRECTORY;
  m_dsaSettings[AppConstants::PROPERTYNAME_USERNAME] = QHostInfo::localHostName();
  m_dsaSettings[AppConstants::PROPERTYNAME_BASEMAP_DIRECTORY] = QStringLiteral("./BasemapData");
  m_dsaSettings[AppConstants::PROPERTYNAME_ELEVATION_DIRECTORY] = QStringLiteral("./ElevationData");
  m_dsaSettings[AppConstants::PROPERTYNAME_SIMULATION_DIRECTORY] = QStringLiteral("./SimulationData");
  m_dsaSettings[LocationController::PROPERTY_NAME_RESOURCE_DIRECTORY] = QStringLiteral("./ResourceData");
  m_dsaSettings[AppConstants::PROPERTYNAME_LOCAL_DATA_PATHS] = QStringList{ QStringLiteral("./OperationalData") };
  m_dsaSettings[AppConstants::PROPERTYNAME_DEFAULT_BASEMAP] = AppConstants::BASEMAP_NAME_TOPOGRAPHIC;
  m_dsaSettings[AppConstants::PROPERTYNAME_DEFAULT_ELEVATION_SOURCE] = QString("%1/CaDEM.tpk").arg(m_dsaSettings[AppConstants::PROPERTYNAME_ELEVATION_DIRECTORY].toString());
  m_dsaSettings[LocationController::PROPERTY_NAME_GPX_FILE] = QString("%1/MontereyMounted.gpx").arg(m_dsaSettings[AppConstants::PROPERTYNAME_SIMULATION_DIRECTORY].toString());
  m_dsaSettings[LocationController::PROPERTY_NAME_SIMULATE_LOCATION] = AppConstants::BOOL_TRUE;
  m_dsaSettings[LocationController::PROPERTY_NAME_CURRENT_LOCATION_Z_OFFSET] = QStringLiteral("10");
  m_dsaSettings[LocationController::PROPERTY_NAME_CURRENT_LOCATION_SURFACE_PLACEMENT] = QStringLiteral("Relative");
  writeDefaultMessageFeeds();
  m_dsaSettings[AppConstants::PROPERTYNAME_COORDINATE_FORMAT] = Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::MGRS_FORMAT;
  m_dsaSettings[AppConstants::PROPERTYNAME_UNIT_OF_MEASUREMENT] = AppConstants::UNIT_METERS;
  m_dsaSettings[AppConstants::PROPERTYNAME_USE_GPS_FOR_ELEVATION] = AppConstants::BOOL_TRUE;
  QJsonObject markupJson;
  markupJson.insert(QStringLiteral("port"), 45680);
  m_dsaSettings[AppConstants::PROPERTYNAME_MARKUP_CONFIG] = markupJson;
  m_dsaSettings[GridController::PROPERTY_NAME_GRID_VISIBLE] = GridController::GRID_VISIBLE_VALUE_DEFAULT;
  m_dsaSettings[GridController::PROPERTY_NAME_GRID_COLOR_SCHEME] = GridController::GRID_COLOR_SCHEME_VALUE_DEFAULT;
  writeDefaultConditions();
  m_dsaSettings[OpenMobileScenePackageController::PACKAGE_DIRECTORY_PROPERTYNAME] = QStringLiteral("./Packages");
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

void DsaController::writeInitialLocation(const Viewpoint& viewpoint)
{
  if (viewpoint.isEmpty())
    return;

  auto initialCamera = viewpoint.camera();
  if (initialCamera.isEmpty())
    return;

  QJsonObject initialLocationJson;
  const QString centerString = initialCamera.location().toJson();
  const QJsonDocument centerDoc = QJsonDocument::fromJson(centerString.toLatin1());
  initialLocationJson.insert( QStringLiteral("center"), centerDoc.object());
  initialLocationJson.insert( QStringLiteral("distance"), 0);
  initialLocationJson.insert( QStringLiteral("heading"), initialCamera.heading());
  initialLocationJson.insert( QStringLiteral("pitch"), initialCamera.pitch());
  initialLocationJson.insert( QStringLiteral("roll"), initialCamera.roll());

  m_dsaSettings[AppConstants::PROPERTYNAME_INITIAL_LOCATION] = initialLocationJson.toVariantMap();
}

Viewpoint DsaController::readInitialLocation() const
{
  return viewpointFromJson(m_dsaSettings[AppConstants::PROPERTYNAME_INITIAL_LOCATION].toJsonObject());
}

void DsaController::updateInitialLocationOnSceneChange(bool isInitialization)
{
  if (!m_scene)
    return;

  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  if (!isInitialization)
    return;

  // This is an initilization scene change, replace the scene's
  // initial location with our current initial location.
  const Viewpoint v = readInitialLocation();
  if (v.isEmpty())
    return;

  // Note use of setViewPoint instead of setInitialLocation. The latter
  // only works if the scene is not loaded, but all MSPK scenes are loaded with
  // the MSPK so it can't be used.
  geoView->setViewpointAsync(v, 0);
}

namespace
{

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

Viewpoint viewpointFromJson(const QJsonObject& initialLocation)
{
  if (initialLocation.isEmpty())
    return Viewpoint{};

  // set the initial center Point from JSON if it is found
  auto centerIt = initialLocation.find("center");
  if (centerIt == initialLocation.constEnd())
    return Viewpoint{};

  const QJsonValue centerVal = centerIt.value();
  const QJsonDocument centerDoc = QJsonDocument(centerVal.toObject());
  const auto newCenter = Point::fromJson(centerDoc.toJson(QJsonDocument::JsonFormat::Compact));

  auto distanceIt = initialLocation.find("distance");
  if (distanceIt == initialLocation.constEnd())
    return Viewpoint{};
  double initialDistance = distanceIt.value().toDouble(0);

  auto headingIt = initialLocation.find("heading");
  if (headingIt == initialLocation.constEnd())
    return Viewpoint{};
  double initialHeading = headingIt.value().toDouble(0.0);

  auto pitchIt = initialLocation.find("pitch");
  if (pitchIt == initialLocation.constEnd())
    return Viewpoint{};
  double initialPitch = pitchIt.value().toDouble(0.0);

  auto rollIt = initialLocation.find("roll");
  if (rollIt == initialLocation.constEnd())
    return Viewpoint{};
  double initialRoll = rollIt.value().toDouble(0.0);

  // Return the initial viewpoint
  const Camera initCamera(geometry_cast<Point>(newCenter), initialDistance, initialHeading, initialPitch, initialRoll);
  Viewpoint initViewpoint(newCenter, initCamera);
  return initViewpoint;
}

QJsonObject defaultViewpoint()
{
  QJsonObject defaultViewpoint;
  const QString point = DsaUtility::montereyCA().toJson();
  const QJsonDocument centerDoc = QJsonDocument::fromJson(point.toLatin1());
  defaultViewpoint.insert( QStringLiteral("center"), centerDoc.object());
  defaultViewpoint.insert( QStringLiteral("distance"), 5000);
  defaultViewpoint.insert( QStringLiteral("heading"), 0.0);
  defaultViewpoint.insert( QStringLiteral("pitch"), 75.0);
  defaultViewpoint.insert( QStringLiteral("roll"), 0.0);
  return defaultViewpoint;
}

} // namespace
} // namespace Dsa

// Signal Documentation

/*!
  \fn void DsaController::errorOccurred(const QString& message, const QString& additionalMessage);

  \brief Signal emitted when an error occurs.

  An error \a message and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */

