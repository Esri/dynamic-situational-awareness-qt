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

TARGET = DSA_MessageSimulator_Qt
TEMPLATE = app

QT += qml quick xml
CONFIG += c++11

INCLUDEPATH += \
    $$PWD/../Shared/messages \
    $$PWD/../Shared/utilities

HEADERS += \
    $$PWD/../Shared/utilities/DataSender.h \
    MessageSimulatorController.h \
    AbstractMessageParser.h \
    CoTMessageParser.h \
    SimulatedMessage.h \
    SimulatedMessageListModel.h \
    GeoMessageParser.h

SOURCES += main.cpp \
    $$PWD/../Shared/utilities/DataSender.cpp \
    AbstractMessageParser.cpp \
    CoTMessageParser.cpp \
    MessageSimulatorController.cpp \
    SimulatedMessage.cpp \
    SimulatedMessageListModel.cpp \
    GeoMessageParser.cpp

RESOURCES += qml/qml.qrc \
    Resources/application.qrc
	
#-------------------------------------------------------------------------------

win32 {
    include (Win/Win.pri)
}

macx {
    include (Mac/Mac.pri)
}
