#-------------------------------------------------
#  Copyright 2016 ESRI
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

TARGET = DSA_Handheld_Qt
TEMPLATE = app

QT += core gui opengl network positioning sensors qml quick xml svg
CONFIG += c++11

# uncomment this line to use a local toolkit build
#CONFIG += LocalToolkitBuild ToolkitBuildUsePrefix

ARCGIS_RUNTIME_VERSION = 100.2.1
include($$PWD/../Shared/arcgisruntime.pri)

INCLUDEPATH += $$PWD/../Shared/ \
    $$PWD/../Shared/alerts \
    $$PWD/../Shared/analysis \
    $$PWD/../Shared/messages \
    $$PWD/../Shared/utilities

HEADERS += \
    AppInfo.h \
    Handheld.h \
    HandheldStyles.h \
    $$PWD/../Shared/*.h \
    $$PWD/../Shared/alerts/*.h \
    $$PWD/../Shared/analysis/*.h \
    $$PWD/../Shared/messages/*.h \
    $$PWD/../Shared/utilities/*.h

SOURCES += \
    main.cpp \
    Handheld.cpp \
    HandheldStyles.cpp \
    $$PWD/../Shared/*.cpp \
    $$PWD/../Shared/alerts/*.cpp \
    $$PWD/../Shared/analysis/*.cpp \
    $$PWD/../Shared/messages/*.cpp \
    $$PWD/../Shared/utilities/*.cpp

RESOURCES += \
    qml/qml.qrc \
    ../Shared/Resources/Resources.qrc \
    ../Shared/Resources/application.qrc \
    ../Shared/qml/shared_qml.qrc

#-------------------------------------------------------------------------------

QML_IMPORT_PATH += $$PWD/../Shared/qml

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
