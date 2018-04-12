
// Copyright 2018 ESRI
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
    id: queryPage

    property bool valid: attributeFieldEdit.length > 0
    property string instruction: "Select attribute"
    property alias attributeField: attributeFieldEdit.text
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    function text() {
        return "have attrribute " + attributeFieldEdit.text + " = ";
    }

    function clear() {
        attributeFieldEdit.text = "";
    }

    Column {
        anchors {
            fill: parent
            margins: 16 * scaleFactor
        }

        Grid {
            rows: 2
            columns: 2
            spacing: 8 * scaleFactor

            Text {
                id: fieldLabel
                text: qsTr("have attribute")
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: attributeFieldEdit
                color: Material.accent
                font {
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    bold: true
                }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                placeholderText: "<enter attribute name>"
            }

            Text {
                text: qsTr("equal to")
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
