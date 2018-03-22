
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

Item {
    id: reportDatePage

    property bool valid: size.length > 0
    property string instruction: "Size of enemy unit"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property string size: smallButton.checked ? "small" :
                                                (mediumButton.checked ? "medium" : "large")

    function clear() {
    }

    Column {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        spacing: 10 * scaleFactor
        leftPadding: 10 * scaleFactor

        ButtonGroup {
            id: sizeGroup
        }

        RadioButton {
            id: smallButton
            text: "Small"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            checked: true
            ButtonGroup.group: sizeGroup
        }

        RadioButton {
            id: mediumButton
            text: "Medium"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: sizeGroup
        }

        RadioButton {
            id: largeButton
            visible: true
            text: "Large"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: sizeGroup
        }
    }
}
