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

include($$PWD/resolvelocaltoolkit.pri)

# Try to use any located local C++ toolkit first. If none
# can be found use the one included with the SDK install

!isEmpty(CppToolkitLocation) {
  # local toolkit found
  include($$CppToolkitLocation/ArcGISRuntimeToolkit.pri) {
    message("Using local C++ toolkit: $$CppToolkitLocation")
    DEFINES += QTRUNTIME_TOOLKIT_STATIC_BUILD
  }
}
else {
  include($$PWD/resolvesdkinstall.pri)
  # use the toolkit included with the SDK installation
  !include($$QtRuntimeSdkPath/sdk/ideintegration/arcgis_runtime_toolkit_cpp.pri) {
    message("Error. Cannot locate ArcGIS Runtime C++ Toolkit PRI file")
  }
}
