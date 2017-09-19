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

TARGET = DSA_Vehicle_Qt
TEMPLATE = app

QT += core gui opengl network positioning sensors qml quick xml
CONFIG += c++11

ARCGIS_RUNTIME_VERSION = 100.2
include($$PWD/arcgisruntime.pri)

INCLUDEPATH += ../Shared/

HEADERS += \
    AppInfo.h \
    Vehicle.h \
    ../Shared/DsaUtility.h \
    ../Shared/BasemapPickerController.h \
    ../Shared/TileCacheListModel.h \
    ../Shared/AddLocalDataController.h \
    ../Shared/DataItemListModel.h \
    ../Shared/DsaController.h \
    ../Shared/GPXLocationSimulator.h \
    ../Shared/LocationController.h \
    ../Shared/Message.h \
    ../Shared/MessageListener.h \
    ../Shared/MessagesOverlay.h

SOURCES += \
    main.cpp \
    Vehicle.cpp \
    ../Shared/DsaUtility.cpp \
    ../Shared/BasemapPickerController.cpp \
    ../Shared/TileCacheListModel.cpp \
    ../Shared/AddLocalDataController.cpp \
    ../Shared/DataItemListModel.cpp \
    ../Shared/DsaController.cpp \
    ../Shared/GPXLocationSimulator.cpp \
    ../Shared/LocationController.cpp \
    ../Shared/Message.cpp \
    ../Shared/MessageListener.cpp \
    ../Shared/MessagesOverlay.cpp

RESOURCES += \
    qml/qml.qrc \
    ../Shared/Resources/Resources.qrc \
    ../Shared/Resources/application.qrc \
    ../Shared/qml/shared_qml.qrc

QML_IMPORT_PATH += $$PWD/../Shared/qml

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

DISTFILES += \
    Resources/qtquickcontrols2.conf
