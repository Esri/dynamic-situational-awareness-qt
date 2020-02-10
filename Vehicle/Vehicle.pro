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

TARGET = DSA_Vehicle_Qt
TEMPLATE = app

QT += core gui opengl network positioning sensors qml quick xml
CONFIG += c++14

ARCGIS_RUNTIME_VERSION = 100.7
include($$PWD/../Shared/build/arcgisruntime.pri)

INCLUDEPATH += $$PWD/../Shared/ \
    $$PWD/../Shared/alerts \
    $$PWD/../Shared/analysis \
    $$PWD/../Shared/CoordinateConversion \
    $$PWD/../Shared/messages \
    $$PWD/../Shared/packages \
    $$PWD/../Shared/utilities \
    $$PWD/../Shared/markup

HEADERS += \
    AppInfo.h \
    Vehicle.h \
    VehicleStyles.h \
    $$PWD/../Shared/*.h \
    $$PWD/../Shared/alerts/*.h \
    $$PWD/../Shared/analysis/*.h \
    $$PWD/../Shared/CoordinateConversion/*.h \
    $$PWD/../Shared/messages/*.h \
    $$PWD/../Shared/packages/*.h \
    $$PWD/../Shared/utilities/*.h \
    $$PWD/../Shared/markup/*.h

SOURCES += \
    main.cpp \
    Vehicle.cpp \
    VehicleStyles.cpp \
    $$PWD/../Shared/*.cpp \
    $$PWD/../Shared/alerts/*.cpp \
    $$PWD/../Shared/analysis/*.cpp \
    $$PWD/../Shared/CoordinateConversion/*.cpp \
    $$PWD/../Shared/messages/*.cpp \
    $$PWD/../Shared/packages/*.cpp \
    $$PWD/../Shared/utilities/*.cpp \
    $$PWD/../Shared/markup/*.cpp

RESOURCES += \
    qml/qml.qrc \
    ../Shared/Resources/Resources.qrc \
    ../Shared/Resources/application.qrc \
    ../Shared/qml/shared_qml.qrc

QML_IMPORT_PATH += $$PWD/../Shared/qml

!android {
  PRECOMPILED_HEADER = $$PWD/../Shared/pch.hpp
  CONFIG += precompile_header
}

#-------------------------------------------------------------------------------

win32 {
    include (Win/Win.pri)
}

macx {
    include (Mac/Mac.pri)
}

ios {
    include (iOS/iOS.pri)
}

android {
    include (Android/Android.pri)
}

DISTFILES += \
    Resources/qtquickcontrols2.conf \
    ../qdoc/dsa-overview.qdoc
