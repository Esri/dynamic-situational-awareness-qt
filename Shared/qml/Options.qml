
// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

Rectangle {
    id: optionsRoot
    color: Material.primary

    OptionsController {
        id: optionsController
    }

    Flickable {
        anchors {
            fill: parent
            margins: 8 * scaleFactor
        }
        contentHeight: optionsColumn.height
        clip: true

        Column {
            id: optionsColumn
            width: parent.width
            spacing: 5 * scaleFactor

            Row {
                height: 40 * scaleFactor
                width: parent.width

                CheckBox {
                    text: "Show Navigation Controls"
                    checked: true
                    onCheckedChanged: {
                        navTool.visible = checked;
                        compass.visible = checked;
                    }
                }
            }

            Row {
                height: 40 * scaleFactor
                width: parent.width

                CheckBox {
                    text: "Location Broadcast"
                    checked: messageFeeds.controller.locationBroadcastEnabled
                    onCheckedChanged: {
                        messageFeeds.controller.locationBroadcastEnabled = checked;
                    }
                }
            }

            Row {
                height: 40 * scaleFactor
                width: parent.width
                spacing: 5 * scaleFactor

                Text {
                    text: "Location Broadcast frequency (ms)"
                    color: Material.foreground
                    font {
                        pixelSize: 10 * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                }

                TextField {
                    width: 50 * scaleFactor
                    text: messageFeeds.controller.locationBroadcastFrequency
                    color: Material.foreground
                    font {
                        pixelSize: 10 * scaleFactor
                        family: DsaStyles.fontFamily
                    }

                    onTextChanged: {
                        messageFeeds.controller.locationBroadcastFrequency = Number(text);
                    }
                }
            }
        }
    }

    Button {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 10 * scaleFactor
        }
        text: "Close"
        onClicked: optionsRoot.visible = false;
    }
}
