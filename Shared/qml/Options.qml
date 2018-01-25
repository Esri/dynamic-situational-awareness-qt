
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
            margins: 10 * scaleFactor
        }
        contentHeight: optionsColumn.height
        clip: true

        Column {
            id: optionsColumn
            width: parent.width
            spacing: 10 * scaleFactor

            Label {
                text: "Map Controls"
                font {
                    pixelSize: 14 * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                    underline: true
                }
                color: Material.foreground
            }


            CheckBox {
                text: "Show navigation controls"
                checked: true
                onCheckedChanged: {
                    // update visibility of UI components
                    navTool.visible = checked;
                    compass.visible = checked;
                }
            }

            CheckBox {
                text: "Show location and elevation"
                checked: true
                onCheckedChanged: {
                    // update visibility of UI component
                    currentLocation.visible = checked;
                }
            }

            CheckBox {
                text: "Use GPS for current elevation display"
                checked: false
                onCheckedChanged: {
                    console.log("TODO");
                }
            }

            Label {
                text: "Formats, Units of measurement, etc"
                font {
                    pixelSize: 14 * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                    underline: true
                }
                color: Material.foreground
            }

            Row {
                width: parent.width
                spacing: 10 * scaleFactor

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Default Coordinate Format"
                    font {
                        pixelSize: 12 * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                    color: Material.foreground
                }

                ComboBox {
                    anchors.verticalCenter: parent.verticalCenter
                    model: optionsController.coordinateFormats
                    onCurrentTextChanged: {
                        optionsController.setCoordinateFormat(currentText);
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 10 * scaleFactor

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Default Unit of Measurement"
                    font {
                        pixelSize: 12 * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                    color: Material.foreground
                }

                ComboBox {
                    anchors.verticalCenter: parent.verticalCenter
                    model: 4
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
