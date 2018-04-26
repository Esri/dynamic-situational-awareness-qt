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

equals($$QtRuntimeSdkPath, "") {
  include($$PWD/resolvesdkinstall.pri)
}

CONFIG(deployment): DEFINES += DEPLOYMENT_BUILD

!include($$QtRuntimeSdkPath/sdk/ideintegration/arcgis_runtime_qml_cpp.pri) {
  message("Error. Cannot locate ArcGIS Runtime PRI file")
}

ios: {
  include(resolvelocaltoolkit.pri)
  !isEmpty(CppToolkitLocation) {
    include($$CppToolkitLocation/../ArcGISRuntimeToolkitPlugin.pri)
  } else {
    !include($$QtRuntimeSdkPath/sdk/toolkit/Plugin/ArcGISRuntimeToolkitPlugin.pri) {
      message("Error. Cannot locate PRI file to import Toolkit as a plugin")
    }
  }
}
