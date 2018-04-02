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

# look in typical locations for the toolkit repo

exists($$PWD/../../../../sdk/toolkit/Plugin/CppApi/ArcGISRuntimeToolkit.pri) {
  CppToolkitLocation = $$absolute_path($$PWD/../../../../sdk/toolkit/Plugin/CppApi)
} else {
  exists($$PWD/../../arcgis-runtime-toolkit-qt/Plugin/CppApi/ArcGISRuntimeToolkit.pri) {
    CppToolkitLocation = $$absolute_path($$PWD/../../arcgis-runtime-toolkit-qt/Plugin/CppApi)
  }
}
