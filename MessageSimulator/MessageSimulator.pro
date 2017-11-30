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

TARGET = DSA_MessageSimulator_Qt
TEMPLATE = app

QT += qml quick
CONFIG += c++11

HEADERS += \
    MessageSimulatorController.h \
    AbstractMessageParser.h \
    CoTMessageParser.h \
    MessageSender.h \
    SimulatedMessage.h \
    SimulatedMessageListModel.h

SOURCES += main.cpp \
    AbstractMessageParser.cpp \
    CoTMessageParser.cpp \
    MessageSender.cpp \
    MessageSimulatorController.cpp \
    SimulatedMessage.cpp \
    SimulatedMessageListModel.cpp

RESOURCES += qml/qml.qrc \
    Resources/application.qrc
	
#-------------------------------------------------------------------------------

win32 {
    include (Win/Win.pri)
}

macx {
    include (Mac/Mac.pri)
}
