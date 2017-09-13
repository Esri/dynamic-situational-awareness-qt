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

QT += core gui opengl network positioning sensors qml quick
CONFIG += c++11

ARCGIS_RUNTIME_VERSION = 100.2
include($$PWD/arcgisruntime.pri)

INCLUDEPATH += ../Shared/

HEADERS += \
    AppInfo.h \
    Handheld.h \
    ../Shared/DsaUtility.h \
    ../Shared/BasemapPickerController.h \
    ../Shared/TileCacheListModel.h \
    ../Shared/DsaController.h

SOURCES += \
    main.cpp \
    Handheld.cpp \
    ../Shared/DsaUtility.cpp \
    ../Shared/BasemapPickerController.cpp \
    ../Shared/TileCacheListModel.cpp \
    ../Shared/DsaController.cpp

RESOURCES += \
    qml/qml.qrc \
    Resources/Resources.qrc \
    Resources/application.qrc \
    ../Shared/qml/shared_qml.qrc

#-------------------------------------------------------------------------------

win32 {
    include (Win/Win.pri)
}

macx {
    include (Mac/Mac.pri)
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
}

ios {
    include (iOS/iOS.pri)
}

android {
    include (Android/Android.pri)
}
