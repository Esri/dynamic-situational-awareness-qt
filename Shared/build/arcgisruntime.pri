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

equals($$QtRuntimeSdkPath, "") {
  include($$PWD/resolvesdkinstall.pri)
}

CONFIG(deployment): DEFINES += DEPLOYMENT_BUILD

!include($$QtRuntimeSdkPath/sdk/ideintegration/arcgis_runtime_qml_cpp.pri) {
  message("Error. Cannot locate ArcGIS Runtime PRI file")
}

ios: {
  !include($$QtRuntimeSdkPath/sdk/toolkit/Plugin/ArcGISRuntimeToolkitPlugin.pri) {
    message("Error. Cannot locate PRI file to import Toolkit as a plugin")
  }
}
