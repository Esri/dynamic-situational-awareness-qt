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

import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick
import QtQuick.Window
import Esri.MessageSimulator

Rectangle {
    id: settingsPage
    clip: true

    property alias xmlFilePath: xmlFilePath.text
    property alias port: portEdit.text

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    ColumnLayout {
        spacing: 16

        Rectangle {
            Layout.preferredWidth: settingsPage.width
            Layout.preferredHeight: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            RowLayout {
                spacing: 10
                width: parent.width

                Button {
                    id: chooseFileBtn
                    Layout.leftMargin: 5
                    enabled: messageSimulatorController.simulationState === MessageSimulatorController.Stopped
                    text: "message file"
                    font.bold: true
                    onClicked: loader.open();
                }


                TextField {
                    id: xmlFilePath
                    Layout.rightMargin: 5
                    Layout.fillWidth: true
                    placeholderText: "please choose a message file (.xml)"
                    text: loader.fileUrl.toString() !== "" ? loader.fileUrl : messageSimulatorController.simulationFile
                    readOnly: true

                    validator: IntValidator{}
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: settingsPage.width
            Layout.preferredHeight: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            RowLayout {
                spacing: 10
                width: parent.width

                Label {
                    id: portLabel
                    Layout.leftMargin: 5
                    text: "UDP Port #"
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                TextField {
                    id: portEdit
                    Layout.rightMargin: 5
                    Layout.fillWidth: true
                    enabled: messageSimulatorController.simulationState === MessageSimulatorController.Stopped
                    text: messageSimulatorController.port !== -1 ? messageSimulatorController.port : ""
                    placeholderText: "8888"
                    height: portLabel.height

                    validator: IntValidator { bottom:0; top: 65535}

                    onTextChanged: {
                        messageSimulatorController.port = Number(text)
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: settingsPage.width
            Layout.preferredHeight: 50 * scaleFactor
            color: "steelblue"
            radius: 4 * scaleFactor

            RowLayout {
                spacing: 10
                width: parent.width

                Slider {
                    id: speedSlider
                    Layout.leftMargin: 5
                   // Layout.preferredWidth: 200
                    enabled: messageSimulatorController.simulationState !== MessageSimulatorController.Running
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
                   // Layout.preferredWidth: 48
                    font.bold: true
                    color: "white"
                    text: speedSlider.value.toFixed(0) + (speedSlider.value.toFixed(0) > 1 ? qsTr(" messages per") : qsTr(" message per"))
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }

                ComboBox {
                    id: messagesSpeedOptions
                    Layout.fillWidth: true
                    Layout.rightMargin: 5
                    Layout.preferredWidth: 48
                    enabled: messageSimulatorController.simulationState !== MessageSimulatorController.Running

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
        }

        CheckBox {
            id: loopCheckBox
            text: "Loop"
            font.bold: true
            checked: messageSimulatorController.simulationLooped
            onCheckedChanged: {
                messageSimulatorController.simulationLooped = checked;
            }
        }
    }

    XmlLoader {
        id: loader
        supportedExtensions: ["xml"]
    }
}

