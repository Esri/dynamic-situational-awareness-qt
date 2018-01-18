import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.0
import QtQuick 2.7
import QtQuick.Window 2.3

Rectangle {
    id: messagesPage
    clip: true

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    ListView {
        id: messagesList
        anchors {
            top: messagesPage.top
            left: messagesPage.left
            right: messagesPage.right
            bottom: adhocMessagePane.top
        }
        clip: true

        model: messageSimulatorController.simulationStarted ? messageSimulatorController.messages : null

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
