# Copyright 2012-2025 Esri
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TARGET = DSA_UnitTests
TEMPLATE = app

QT += core gui opengl xml testlib network positioning sensors multimedia quick qml xml concurrent
CONFIG += c++17

ARCGIS_RUNTIME_VERSION = 200.8.0
DEFINES += ARCGIS_MAPS_SDK_VERSION=$$ARCGIS_RUNTIME_VERSION
include($$PWD/../Shared/build/arcgisruntime.pri)

include($$PWD/../3rdparty/zlib-ng/zlib_minizip_ng.pri)

INCLUDEPATH += \
    $$PWD/3rdparty \
    $$PWD/../Shared \
    $$PWD/../Shared/alerts \
    $$PWD/../Shared/analysis \
    $$PWD/../Shared/messages \
    $$PWD/../Shared/packages \
    $$PWD/../Shared/utilities \
    $$PWD/../Shared/markup \
    $$PWD/../Shared/configurations

HEADERS += \
    $$files($$PWD/test_*.h) \
    $$files($$PWD/../Shared/*.h) \
    $$files($$PWD/../Shared/alerts/*.h) \
    $$files($$PWD/../Shared/analysis/*.h) \
    $$files($$PWD/../Shared/messages/*.h) \
    $$files($$PWD/../Shared/packages/*.h) \
    $$files($$PWD/../Shared/utilities/*.h) \
    $$files($$PWD/../Shared/markup/*.h) \
    $$files($$PWD/../Shared/configurations/*.h)

SOURCES += \
    main.cpp \
    $$files($$PWD/test_*.cpp) \
    $$files($$PWD/../Shared/*.cpp) \
    $$files($$PWD/../Shared/alerts/*.cpp) \
    $$files($$PWD/../Shared/analysis/*.cpp) \
    $$files($$PWD/../Shared/messages/*.cpp) \
    $$files($$PWD/../Shared/packages/*.cpp) \
    $$files($$PWD/../Shared/utilities/*.cpp) \
    $$files($$PWD/../Shared/markup/*.cpp) \
    $$files($$PWD/../Shared/configurations/*.cpp)
