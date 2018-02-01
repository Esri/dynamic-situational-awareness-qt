#-------------------------------------------------
#  Copyright 2017 ESRI
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

# uncomment this line to use a local toolkit build
#CONFIG += LocalToolkitBuild ToolkitBuildUsePrefix

ARCGIS_RUNTIME_VERSION = 100.2.1
include($$PWD/../Shared/arcgisruntime.pri)

INCLUDEPATH += $$PWD/../Shared/ \
    $$PWD/../Shared/alerts


HEADERS += \
    AppInfo.h \
    Vehicle.h \
    VehicleStyles.h \
    $$PWD/../Shared/*.h \
    $$PWD/../Shared/alerts/*.h \
    ../Shared/alerts/AttributeEqualsAlertCondition.h \
    ../Shared/alerts/AttributeEqualsAlertConditionData.h

SOURCES += \
    main.cpp \
    Vehicle.cpp \
    VehicleStyles.cpp \
    $$PWD/../Shared/*.cpp \
    $$PWD/../Shared/alerts/*.cpp \
    ../Shared/alerts/AttributeEqualsAlertCondition.cpp \
    ../Shared/alerts/AttributeEqualsAlertConditionData.cpp

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
}

ios {
    include (iOS/iOS.pri)
}

android {
    include (Android/Android.pri)
}

DISTFILES += \
    Resources/qtquickcontrols2.conf
