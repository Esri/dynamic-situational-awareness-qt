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

TARGET = DSA_MessageSimulator_Qt
TEMPLATE = app

QT += qml quick xml
CONFIG += c++17

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
