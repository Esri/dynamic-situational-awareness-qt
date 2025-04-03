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
import QtQuick
import QtQuick.Window
import Esri.MessageSimulator

Rectangle {
    id: messagesPage
    clip: true

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    ListView {
        id: messagesList
        anchors {
            top: messagesPage.top
            left: messagesPage.left
            right: messagesPage.right
            bottom: adhocMessagePane.top
        }
        clip: true

        model: messageSimulatorController.simulationState === MessageSimulatorController.Running ?
                   messageSimulatorController.messages : null

        header: Component {
            Rectangle {
                width: messagesList.width
                color: "steelblue"
                height: 32 * scaleFactor

                Row {

                    spacing: 16 * scaleFactor

                    Text {
                        id: messageFormatHeader
                        text: qsTr("message\nformat")
                        width: messagesList.width * 0.2
                        font.bold: true
                        color: "white"
                    }

                    Text {
                        id: messageIdHeader
                        text: "message\nid"
                        width: messagesList.width * 0.2
                        font.bold: true
                        color: "white"
                    }

                    Text {
                        id: messageActionHeader
                        text: "message\naction"
                        width: messagesList.width * 0.2
                        font.bold: true
                        color: "white"
                    }

                    Text {
                        id: symbolIdHeader
                        text: "symbol\nid"
                        width: messagesList.width * 0.2
                        font.bold: true
                        color: "white"
                    }
                }
            }
        }

        delegate: Component {
            Rectangle {
                width: messagesList.width
                color: index % 2 === 0 ? "lightgrey" :
                                         "white"
                height: 24 * scaleFactor

                Row {
                    id: delRow
                    spacing: 16 * scaleFactor

                    Text {
                        id: messageFormatText
                        text: messageFormat
                        width: messagesList.width * 0.2
                        clip: true
                        elide: Text.ElideRight
                    }

                    Text {
                        id: messageIdText
                        text: messageId
                        width: messagesList.width * 0.2
                        clip: true
                        elide: Text.ElideRight
                    }

                    Text {
                        id: messageActionText
                        text: messageAction
                        width: messagesList.width * 0.2
                        clip: true
                        elide: Text.ElideRight
                    }

                    Text {
                        id: symbolIdText
                        text: symbolId
                        width: messagesList.width * 0.2
                        clip: true
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }

    Rectangle {
        id: adhocMessagePane
        anchors {
            left: parent.left
            bottom: parent.bottom
        }

        width: messagesPage.width
        height: 50 * scaleFactor
        color: "steelblue"
        radius: 4 * scaleFactor

        TextField {
            id: adhocMessageEdit

            anchors {
                top: parent.top
                left: parent.left
                margins: 8 * scaleFactor
                right: sendMessageButton.left
            }
            placeholderText: "Enter message (CoT XML, JSON, etc...)"
        }

        Button {
            id: sendMessageButton
            anchors {
                top: parent.top
                right: parent.right
                margins: 8 * scaleFactor
            }
            width: 64 * scaleFactor
            text: "send"
            enabled: adhocMessageEdit.text !== ""
            font.bold: true

            onClicked: {
                messageSimulatorController.sendMessage(adhocMessageEdit.text);
            }
        }
    }
}
