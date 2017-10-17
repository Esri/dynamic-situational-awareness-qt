#-------------------------------------------------
#  Copyright 2017 ESRI
#
#  All rights reserved under the copyright laws of the United States
#  and applicable international laws, treaties, and conventions.
#
#  You may freely redistribute and use this sample code, with or
#  without modification, provided you include the original copyright
#  notice and use restrictions.
#
#  See the Sample code usage restrictions document for further information.
#-------------------------------------------------

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

CONFIG(deployment): DEFINES += DEPLOYMENT_BUILD

!include($$priLocation/sdk/ideintegration/arcgis_runtime_qml_cpp.pri) {
  message("Error. Cannot locate ArcGIS Runtime PRI file")
}

!CONFIG(LocalToolkitBuild) {
  !include($$priLocation/sdk/ideintegration/arcgis_runtime_toolkit_cpp.pri) {
    message("Error. Cannot locate ArcGIS Runtime C++ Toolkit PRI file")
  }
}
else {
  # adjust this path to your local build area
  !include($$PWD/../../../sdk/toolkit/Plugin/CppApi/ArcGISRuntimeToolkit.pri) {
    message("Error. Cannot locate local build ArcGIS Runtime C++ Toolkit PRI file.")
    message("You may need to adjust the include path to your local build area.")
  }
}

ios: {
  !include($$priLocation/sdk/toolkit/Plugin/ArcGISRuntimeToolkitPlugin.pri) {
    message("Error. Cannot locate PRI file to import Toolkit as a plugin")
  }
}
