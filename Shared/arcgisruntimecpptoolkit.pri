#-------------------------------------------------
#  Copyright 2018 ESRI
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

include($$PWD/resolvesdkinstall.pri)

!CONFIG(LocalToolkitBuild) {
  !include($$QtRuntimeSdkPath/sdk/ideintegration/arcgis_runtime_toolkit_cpp.pri) {
    message("Error. Cannot locate ArcGIS Runtime C++ Toolkit PRI file")
  }
}
else {
  # adjust this path to your local build area
  !include($$PWD/../../../sdk/toolkit/Plugin/CppApi/ArcGISRuntimeToolkit.pri):
  !include($$PWD/../../arcgis-runtime-toolkit-qt/Plugin/CppApi/ArcGISRuntimeToolkit.pri) {
    message("Error. Cannot locate local build ArcGIS Runtime C++ Toolkit PRI file.")
    message("You may need to adjust the include path to your local build area.")
  }
}
