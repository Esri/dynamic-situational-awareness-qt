
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

    // Add the Options Controller Class
    OptionsController {
        id: optionsController
    }

    // Create a flickable column so that n number of options can be added
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
                id: toolbarLabel
                text: "Settings"
                font {
                    pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
                    family: DsaStyles.fontFamily
                }
                color: Material.foreground
            }

            Label {
                text: "Map Options"
                font {
                    family: DsaStyles.fontFamily
                    underline: true
                    pixelSize: DsaStyles.titleFontPixelSize * 0.75
                }
                color: Material.foreground
            }

            // Toggle navigation controls
            CheckBox {
                text: "Show navigation controls"
                checked: true
                onCheckedChanged: {
                    // update visibility of UI components
                    navTool.visible = checked;
                    compass.visible = checked;
                }
            }

            // Toggle location/elevation overlay
            CheckBox {
                text: "Show location and elevation"
                checked: true
                onCheckedChanged: {
                    // update visibility of UI component
                    currentLocation.visible = checked;
                }
            }

            Label {
                text: "Location Options"
                font {
                    family: DsaStyles.fontFamily
                    underline: true
                    pixelSize: DsaStyles.titleFontPixelSize * 0.75
                }
                color: Material.foreground
            }

            // Whether to use GPS for the location/elevation display or not.
            // The alternative is the use the Scene's base surface.
            CheckBox {
                id: useGPS
                text: "Use GPS for current elevation display"
                checked: optionsController.useGpsForElevation
                onCheckedChanged: optionsController.useGpsForElevation = checked
            }


            CheckBox {
                text: "Location Broadcast"
                checked: messageFeeds.controller.locationBroadcastEnabled
                onCheckedChanged: messageFeeds.controller.locationBroadcastEnabled = checked
            }

            Row {
                height: 40 * scaleFactor
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

                    validator: IntValidator { bottom:0 }

                    onTextChanged: messageFeeds.controller.locationBroadcastFrequency = Number(text)
                }
            }

            Label {
                text: "General Options"
                font {
                    family: DsaStyles.fontFamily
                    underline: true
                    pixelSize: DsaStyles.titleFontPixelSize * 0.75
                }
                color: Material.foreground
            }

            // Change the default coordinate formats between DMS, USNG, MGRS, etc.
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
                    Component.onCompleted: currentIndex = optionsController.initialFormatIndex
                    onCurrentTextChanged: optionsController.setCoordinateFormat(currentText);
                }
            }

            // Change the default units between feet and meters
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
                    model: optionsController.units
                    Component.onCompleted: currentIndex = optionsController.initialUnitIndex
                    onCurrentTextChanged: optionsController.setUnitOfMeasurement(currentText)
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
