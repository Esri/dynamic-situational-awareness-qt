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

#ifndef APPCONSTANTS_H
#define APPCONSTANTS_H

// Qt headers
#include <QString>

namespace Dsa {

class AppConstants {
public:
  static inline const QString PROPERTYNAME_ROOT_DATA_DIRECTORY = QStringLiteral("RootDataDirectory");
  static inline const QString PROPERTYNAME_BASEMAP_DIRECTORY = QStringLiteral("BasemapDirectory");
  static inline const QString PROPERTYNAME_ELEVATION_DIRECTORY = QStringLiteral("ElevationDirectory");
  static inline const QString PROPERTYNAME_PACKAGE_DIRECTORY = QStringLiteral("PackageDirectory");
  static inline const QString PROPERTYNAME_RESOURCE_DIRECTORY = QStringLiteral("ResourceDirectory");
  static inline const QString PROPERTYNAME_SIMULATION_DIRECTORY = QStringLiteral("SimulationDirectory");
  static inline const QString PROPERTYNAME_DEFAULT_ELEVATION_SOURCE = QStringLiteral("DefaultElevationSource");
  static inline const QString PROPERTYNAME_LOCAL_DATA_PATHS = QStringLiteral("LocalDataPaths");

  static inline const QString PROPERTYNAME_USERNAME = QStringLiteral("UserName");
  static inline const QString PROPERTYNAME_LAYERS = QStringLiteral("Layers");
  static inline const QString PROPERTYNAME_CURRENT_SCENE = QStringLiteral("CurrentPackage");
  static inline const QString PROPERTYNAME_SCENE_INDEX = QStringLiteral("SceneIndex");
  static inline const QString PROPERTYNAME_INITIAL_LOCATION = QStringLiteral("InitialLocation");
  static inline const QString PROPERTYNAME_LAYER_NAME_SCENEVIEW_LOCATION = QStringLiteral("SceneViewLocationOverlay");
  static inline const QString PROPERTYNAME_GPX_FILE = QStringLiteral("GpxFile");
  static inline const QString PROPERTYNAME_COORDINATE_FORMAT = QStringLiteral("CoordinateFormat");
  static inline const QString PROPERTYNAME_USE_GPS_FOR_ELEVATION = QStringLiteral("UseGpsForElevation");
  static inline const QString PROPERTYNAME_MARKUP_CONFIG = QStringLiteral("MarkupConfig");

  static inline const QString PROPERTYNAME_DEFAULT_BASEMAP = QStringLiteral("DefaultBasemap");
  static inline const QString BASEMAP_NAME_TOPOGRAPHIC = QStringLiteral("topographic");

  static inline const QString PROPERTYNAME_UNIT_OF_MEASUREMENT = QStringLiteral("UnitOfMeasurement");
  static inline const QString UNIT_METERS = QStringLiteral("meters");
  static inline const QString UNIT_FEET = QStringLiteral("feet");

  static inline const QString BOOL_TRUE = QStringLiteral("true");
};

} // Dsa

#endif // APPCONSTANTS_H
