
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
                    pixelSize: DsaStyles.titleFontPixelSize * scaleFactor * 1.5
                    family: DsaStyles.fontFamily
                }
                color: Material.foreground
            }

            Label {
                text: "Map Controls"
                font {
                    family: DsaStyles.fontFamily
                    underline: true
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

            // Whether to use GPS for the location/elevation display or not.
            // The alternative is the use the Scene's base surface.
            CheckBox {
                id: useGPS
                text: "Use GPS for current elevation display"
                onCheckedChanged: optionsController.useGpsForElevation = checked
                Component.onCompleted: checked = optionsController.useGpsForElevation
                // Checking/Unchecking breaks the binding, so a connection is set up to re-bind
                // More info - https://stackoverflow.com/questions/23860270/binding-checkbox-checked-property-with-a-c-object-q-property
                Connections {
                    target: optionsController
                    onUseGpsForElevationChanged: useGPS.checked = optionsController.useGpsForElevation
                }
            }

            Label {
                text: "Formats, Units of measurement, etc"
                font {
                    family: DsaStyles.fontFamily
                    underline: true
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
