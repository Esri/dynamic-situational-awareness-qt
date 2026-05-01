################################################################################
#  Copyright 2012-2018 Esri
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
################################################################################

CONFIG(deployment): DEFINES += DEPLOYMENT_BUILD

ARCGIS_RUNTIME_VERSION = 300.0.0
DEFINES += ARCGIS_MAPS_SDK_VERSION=$$ARCGIS_RUNTIME_VERSION

# include toolkit and calcite resources
ARCGIS_MAPS_SDK_TOOLKIT_PATH = $$PWD/../../arcgis-maps-sdk-toolkit-qt
include($$ARCGIS_MAPS_SDK_TOOLKIT_PATH/uitools/toolkitcpp/toolkitcpp.pri)
RESOURCES += $$ARCGIS_MAPS_SDK_TOOLKIT_PATH/calcite/Calcite/calcite.qrc
QML_IMPORT_PATH += $$ARCGIS_MAPS_SDK_TOOLKIT_PATH/calcite

contains(QMAKE_HOST.os, Windows):{
  iniPath = $$(ALLUSERSPROFILE)\EsriRuntimeQt\ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
}
else {
  userHome = $$system(echo $HOME)
  iniPath = $${userHome}/.config/EsriRuntimeQt/ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
}
iniLine = $$cat($${iniPath}, "lines")
dirPath = $$find(iniLine, "InstallDir")
cleanDirPath = $$replace(dirPath, "InstallDir=", "")
priLocation = $$replace(cleanDirPath, '"', "")
!include($$priLocation/sdk/ideintegration/arcgis_runtime_qml_cpp.pri) {
  message("Error. Cannot locate ArcGIS Runtime PRI file")
}
