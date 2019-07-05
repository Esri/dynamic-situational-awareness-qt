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

include($$PWD/resolvelocaltoolkit.pri)

# Try to use any located local C++ toolkit first. If none
# can be found use the one included with the SDK install

!isEmpty(CppToolkitLocation) {
  # local toolkit found
  CONFIG += TOOLKIT_STATIC_BUILD
  include($$CppToolkitLocation/ArcGISRuntimeToolkit.pri) {
    message("Using local C++ toolkit: $$CppToolkitLocation")
    DEFINES += QTRUNTIME_TOOLKIT_STATIC_BUILD
  }
}
else {
  error("Error. Cannot locate ArcGIS Runtime C++ Toolkit files. Toolkit repo must be cloned locally.")
}
