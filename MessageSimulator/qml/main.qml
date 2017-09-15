import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.3
import Esri.MessageSimulator 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: qsTr("Message Simulator")

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    MessageSimulatorController {
        id: messageSimulatorController

        onSimulationStartedChanged: {
            loggingPage.loggingText += "\n" + (simulationStarted ? qsTr("Started ") : qsTr("Stopped ")) + new Date().toLocaleString();
            if (simulationStarted) {
                loggingPage.loggingText += "\n" + qsTr("UDP broadcast port: ") + port;
                loggingPage.loggingText += "\n" + qsTr("Sending ") + messageFrequency + qsTr(" message per ") + messageSimulatorController.fromTimeUnit(timeUnit);
            }
        }

        onSimulationPausedChanged: {
            if (simulationStarted) {
                loggingPage.loggingText += "\n" + (simulationPaused ? qsTr("Paused ") : qsTr("Resumed ")) + new Date().toLocaleString();
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
        enabled: messageSimulatorController.simulationStarted
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
        text: messageSimulatorController.simulationPaused ? qsTr("resume") :
                                                            messageSimulatorController.simulationStarted ? qsTr("pause") : qsTr("start")
        font.bold: true
        font.pixelSize: 14 * scaleFactor

        onClicked: {
            if (messageSimulatorController.simulationPaused) {
                messageSimulatorController.resumeSimulation();
            } else if (messageSimulatorController.simulationStarted) {
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
