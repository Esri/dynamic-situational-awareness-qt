/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.3
import Esri.MessageSimulator 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: qsTr("Message Simulator")

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    MessageSimulatorController {
        id: messageSimulatorController

        onSimulationStateChanged: {
            var simulationStateStr = simulationState === MessageSimulatorController.Running ? qsTr("running ") :
                                                                                              simulationState === MessageSimulatorController.Paused ?
                                                                                                  qsTr("paused ") : qsTr("stopped ")
            loggingPage.loggingText += "\n Simulation " + simulationState + new Date().toLocaleString();

            if (simulationState === MessageSimulatorController.Running) {
                loggingPage.loggingText += "\n" + qsTr("UDP broadcast port: ") + port;
                loggingPage.loggingText += "\n" + qsTr("Sending ") + messageFrequency + qsTr(" message per ") + messageSimulatorController.fromTimeUnit(timeUnit);
            }
        }

        onErrorOccurred: {
            loggingPage.loggingText += "\n" + error;
        }
    }

    Text {
        id: titleText
        font.bold: true
        anchors {
            margins: 16 * scaleFactor
            top: parent.top
            left: parent.left
        }

        text: view.currentIndex === 0 ? qsTr("Settings:") :
                                        ( view.currentIndex === 1 ? qsTr("Messages:") :
                                                                    qsTr("Log:"))
    }

    SwipeView {
        id: view

        currentIndex: 0
        anchors {
            margins: 16 * scaleFactor
            top: titleText.bottom
            bottom: divider.top
            left: parent.left
            right: parent.right
        }
        clip: true

        SettingsPage {
            id: settingsPage

            onXmlFilePathChanged: {
                loggingPage.loggingText += qsTr("\nMessages file: ") + xmlFilePath;
            }
        }

        MessagesPage {
            id: messagesPage
        }

        LoggingPage {
            id: loggingPage

        }
    }

    Rectangle {
        id: divider
        anchors {
            bottom: startButton.top
            left: parent.left
            right: parent.right
            margins: 32 * scaleFactor
        }
        height: 2 * scaleFactor
        color: "darkgrey"
    }

    Button {
        id: stopButton

        anchors{
            right: parent.horizontalCenter
            bottom:indicator.top
            margins: 16 * scaleFactor
        }
        enabled: messageSimulatorController.simulationState === MessageSimulatorController.Running ||
                 messageSimulatorController.simulationState === MessageSimulatorController.Paused
        text: qsTr("stop")
        font.bold: true
        font.pixelSize: startButton.font.pixelSize

        onClicked: {
            messageSimulatorController.stopSimulation();
        }
    }

    Button {
        id: startButton

        anchors{
            left: parent.horizontalCenter
            bottom:indicator.top
            margins: 16 * scaleFactor
        }
        enabled: settingsPage.xmlFilePath.length > 0 && settingsPage.port.length > 0
        text: messageSimulatorController.simulationState === MessageSimulatorController.Paused ?
                  qsTr("resume") :
                  messageSimulatorController.simulationState === MessageSimulatorController.Running ?
                      qsTr("pause") :
                      qsTr("start")
        font.bold: true
        font.pixelSize: 14 * scaleFactor

        onClicked: {
            if (messageSimulatorController.simulationState === MessageSimulatorController.Paused) {
                messageSimulatorController.resumeSimulation();
            } else if (messageSimulatorController.simulationState === MessageSimulatorController.Running) {
                messageSimulatorController.pauseSimulation();
            } else {
                messageSimulatorController.startSimulation(settingsPage.xmlFilePath);
            }
        }
    }

    Button {
        anchors {
            bottom: parent.bottom
            top: startButton.bottom
            left: parent.left
            margins: 8 * scaleFactor
        }

        height: 20 * scaleFactor
        width: height

        text: "<"
        font.bold: true
        enabled: view.currentIndex > 0

        onClicked: {
            view.decrementCurrentIndex();
        }
    }

    PageIndicator {
        id: indicator

        count: view.count
        currentIndex: view.currentIndex

        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    Button {
        anchors {
            bottom: parent.bottom
            top: startButton.bottom
            right: parent.right
            margins: 8 * scaleFactor
        }

        height: 20 * scaleFactor
        width: height

        text: ">"
        font.bold: true
        enabled: view.currentIndex < (view.count -1)

        onClicked: {
            view.incrementCurrentIndex();
        }
    }
}
