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
  inline static const QString PROPERTYNAME_ROOT_DATA_DIRECTORY = QStringLiteral("RootDataDirectory");
  inline static const QString PROPERTYNAME_BASEMAP_DIRECTORY = QStringLiteral("BasemapDirectory");
  inline static const QString PROPERTYNAME_ELEVATION_DIRECTORY = QStringLiteral("ElevationDirectory");
  inline static const QString PROPERTYNAME_PACKAGE_DIRECTORY = QStringLiteral("PackageDirectory");
  inline static const QString PROPERTYNAME_RESOURCE_DIRECTORY = QStringLiteral("ResourceDirectory");
  inline static const QString PROPERTYNAME_SIMULATION_DIRECTORY = QStringLiteral("SimulationDirectory");
  inline static const QString PROPERTYNAME_DEFAULT_ELEVATION_SOURCE = QStringLiteral("DefaultElevationSource");
  inline static const QString PROPERTYNAME_LOCAL_DATA_PATHS = QStringLiteral("LocalDataPaths");

  inline static const QString PROPERTYNAME_USERNAME = QStringLiteral("UserName");
  inline static const QString PROPERTYNAME_LAYERS = QStringLiteral("Layers");
  inline static const QString PROPERTYNAME_CURRENT_SCENE = QStringLiteral("CurrentPackage");
  inline static const QString PROPERTYNAME_SCENE_INDEX = QStringLiteral("SceneIndex");
  inline static const QString PROPERTYNAME_INITIAL_LOCATION = QStringLiteral("InitialLocation");
  inline static const QString PROPERTYNAME_LAYER_NAME_SCENEVIEW_LOCATION = QStringLiteral("SceneViewLocationOverlay");
  inline static const QString PROPERTYNAME_GPX_FILE = QStringLiteral("GpxFile");
  inline static const QString PROPERTYNAME_COORDINATE_FORMAT = QStringLiteral("CoordinateFormat");
  inline static const QString PROPERTYNAME_USE_GPS_FOR_ELEVATION = QStringLiteral("UseGpsForElevation");
  inline static const QString PROPERTYNAME_MARKUP_CONFIG = QStringLiteral("MarkupConfig");

  inline static const QString PROPERTYNAME_DEFAULT_BASEMAP = QStringLiteral("DefaultBasemap");
  inline static const QString BASEMAP_NAME_TOPOGRAPHIC = QStringLiteral("topographic");

  inline static const QString PROPERTYNAME_UNIT_OF_MEASUREMENT = QStringLiteral("UnitOfMeasurement");
  inline static const QString UNIT_METERS = QStringLiteral("meters");
  inline static const QString UNIT_FEET = QStringLiteral("feet");

  inline static const QString BOOL_TRUE = QStringLiteral("true");
};

} // Dsa

#endif // APPCONSTANTS_H
