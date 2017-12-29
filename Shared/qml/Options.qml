
// Copyright 2016 ESRI
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaToolBase {
    id: optionsRoot
    title: "Options"
    width: 272 * scaleFactor

    OptionsController {
        id: optionsController
    }

    Flickable {
        anchors {
            top: optionsRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
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

                Label {
                    width: parent.width * 0.8
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Simulate Location"
                    color: Material.foreground
                    elide: Text.ElideRight
                }

                CheckBox {
                    id: simulateCheckbox
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: optionsController.locationControllerReady
                    checked: optionsController.simulateLocation
                    onCheckedChanged: optionsController.simulateLocation = checked
                }
            }

            Row {
                height: 40 * scaleFactor
                width: parent.width
                spacing: 10 * scaleFactor
                visible: simulateCheckbox.checked

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Simulation File"
                    color: Material.foreground
                    elide: Text.ElideRight
                }

                TextField {
                    anchors.verticalCenter: parent.verticalCenter
                    text: optionsController.simulationFile
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
        text: "Apply"
    }
}
