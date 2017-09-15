
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

Rectangle {
    id: locationItem

    signal closed();

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    color: Material.background

    LocationController {
        id: locationController
        enabled: enabledCheckBox.checked;
        simulated: simCheckBox.checked
    }

    Rectangle {
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
        }

        color: Material.primary
        width: parent.width / 4
        height: parent.height / 3;
        Column {
            Row {
                CheckBox {
                    id: enabledCheckBox
                    text: "Enabled"
                }
            }

            Row {
                CheckBox {
                    id: simCheckBox
                    text: "Simulated"
                }
            }

            Row {
                Label {
                    Text {
                        visible: locationController.simulated
                        text: locationController.gpxFilePath
                        color: Material.foreground
                        font.bold: true
                    }
                }
            }
        }
    }
}
