
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

            Label {
                text: "Map Controls"
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                }
                color: Material.foreground
            }


            CheckBox {
                text: "Show Navigation Controls"
                checked: true
                onCheckedChanged: {
                    navTool.visible = checked;
                    compass.visible = checked;
                }
            }

            CheckBox {
                text: "Show Location and Elevation"
                checked: true
                onCheckedChanged: {
                    currentLocation.visible = checked;
                }
            }

            Label {
                text: "Units of Measurement, Formats, etc"
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                }
                color: Material.foreground
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
