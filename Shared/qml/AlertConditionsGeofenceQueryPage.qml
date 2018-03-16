
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
    id: queryPage

    property bool valid: withinAreaRb.checked || withinDistanceSB.value > 0
    property string instruction: "Set query"
    property bool isWithinDistance: withinDistanceRb.checked
    property bool isWithinArea: withinAreaRb.checked
    property real distance: withinDistanceSB.value
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    function text() {
        if (withinAreaRb.checked) {
            return " are within area of";
        }
        else if (withinDistanceRb.checked) {
            return " are within " + withinDistanceSB.value + " m of";
        }
    }

    function clear() {
        withinDistanceSB.value = 50;
        withinAreaRb.checked = true;
    }

    Column {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        ButtonGroup {
            id: spatialOperationGroup
        }

        RadioButton {
            id: withinAreaRb
            text: "are within area of"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            checked: true
            ButtonGroup.group: spatialOperationGroup
        }

        Row {
            spacing: 8 * scaleFactor
            RadioButton {
                id: withinDistanceRb
                text: checked ? "are within" : "are within distance of"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: spatialOperationGroup
            }

            SpinBox {
                id: withinDistanceSB
                anchors.verticalCenter: withinDistanceRb.verticalCenter
                visible: withinDistanceRb.checked
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                width: 128 * scaleFactor
                editable: true
                value: 50
                from: 0
                to: 9999
            }

            Text {
                anchors.verticalCenter: withinDistanceRb.verticalCenter
                visible: withinDistanceRb.checked
                text: qsTr("m of")
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
