import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick 2.7
import QtQuick.Window 2.3
import Esri.MessageSimulator 1.0

Rectangle {
    id: settingsPage
    clip: true

    property alias xmlFilePath: xmlFilePath.text
    property alias port: portEdit.text

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    Column {
        spacing: 16

        Rectangle {
            width: settingsPage.width
            height: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            Button {
                id: chooseFileBtn

                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 8 * scaleFactor
                }

                enabled: !messageSimulatorController.simulationPaused && !messageSimulatorController.simulationStarted
                text: "message\nfile"
                font.bold: true
                width: 64 * scaleFactor
                onClicked: loader.open();
            }

            TextField {
                id: xmlFilePath
                anchors {
                    top: parent.top
                    left: chooseFileBtn.right
                    margins: 8 * scaleFactor
                    right: parent.right
                }
                height: chooseFileBtn.height

                placeholderText: "please choose a message file (.xml)"
                text: loader.fileUrl.toString() !== "" ? loader.fileUrl : messageSimulatorController.simulationFile
                readOnly: true

                validator: IntValidator{}
            }

        }

        Rectangle {
            width: settingsPage.width
            height: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            Label {
                id: portLabel
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    margins: 8 * scaleFactor
                }
                width: 64 * scaleFactor

                text: "port\n(udp)"
                font.bold: true
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: portEdit

                anchors {
                    top: parent.top
                    left: portLabel.right
                    margins: 8 * scaleFactor
                    right: parent.right
                }
                enabled: !messageSimulatorController.simulationPaused && !messageSimulatorController.simulationStarted
                text: messageSimulatorController.port !== -1 ? messageSimulatorController.port : ""
                placeholderText: "8888"
                height: portLabel.height

                validator: IntValidator { bottom:0; top: 65535}

                onTextChanged: {
                    messageSimulatorController.port = Number(text)
                }
            }
        }

        Rectangle {
            width: settingsPage.width
            height: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            Slider {
                id: speedSlider
                anchors {
                    left: parent.left
                    right: speedLabel.left
                    verticalCenter: parent.verticalCenter
                    margins: 30 * scaleFactor
                }
                enabled: !messageSimulatorController.simulationStarted
                orientation: Qt.Horizontal
                from: 1
                to: 500
                value: messageSimulatorController.messageFrequency
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    messageSimulatorController.messageFrequency = value;
                }
            }

            Label {
                id: speedLabel

                anchors {
                    right: messagesSpeedOptions.left
                    verticalCenter: parent.verticalCenter
                    margins: 8 * scaleFactor
                }
                width: 48 * scaleFactor

                font.bold: true
                color: "white"
                text: speedSlider.value.toFixed(0) + (speedSlider.value.toFixed(0) > 1 ? qsTr(" messages per") : qsTr(" message per"))
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
            }

            ComboBox {
                id: messagesSpeedOptions
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: 8 * scaleFactor
                }
                height: 30 * scaleFactor
                width: 100 * scaleFactor
                enabled: !messageSimulatorController.simulationStarted

                onCurrentTextChanged: {
                    var timeUnit = messageSimulatorController.toTimeUnit(currentText);
                    messageSimulatorController.timeUnit = timeUnit;
                }

                Component.onCompleted: {
                    var timeUnit = messageSimulatorController.timeUnit;
                    model = [qsTr("second"), qsTr("minute"), qsTr("hour")];

                    switch(timeUnit)
                    {
                    case MessageSimulatorController.Seconds:
                        currentIndex = 0;
                        break;
                    case MessageSimulatorController.Minutes:
                        currentIndex = 1;
                        break;
                    case MessageSimulatorController.Hours:
                        currentIndex = 2;
                        break;
                    default:
                        currentIndex = 0;
                        break;
                    }
                }
            }
        }

        Rectangle {
            width: settingsPage.width
            height: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            Label {
                id: loopLabel
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    margins: 8 * scaleFactor
                }
                width: 64 * scaleFactor

                text: "loop"
                font.bold: true
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            CheckBox {
                id: loopCheckBox
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: loopLabel.right
                    margins: 8 * scaleFactor
                }

                font.bold: true
                checked: messageSimulatorController.simulationLooped

                onCheckedChanged: {
                    messageSimulatorController.simulationLooped = checked;
                }
            }
        }
    }

    XmlLoader {
        id: loader
        supportedExtensions: ["xml"]
    }
}

