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

TEMPLATE = app

QT += core gui opengl network positioning sensors qml quick
CONFIG += c++11

ARCGIS_RUNTIME_VERSION = 100.2
include($$PWD/arcgisruntime.pri)

HEADERS += \
    AppInfo.h \
    DSA_Vehicle.h

SOURCES += \
    main.cpp \
    DSA_Vehicle.cpp

RESOURCES += \
    qml/qml.qrc \
    Resources/Resources.qrc

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
