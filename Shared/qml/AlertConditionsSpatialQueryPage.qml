/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1

Item {
    id: queryPage

    property bool valid: withinAreaRb.checked || withinDistanceSB.value > 0
    property string instruction: "Set query"
    property bool isWithinDistance: withinDistanceRb.checked
    property bool isWithinArea: withinAreaRb.checked
    property real distance: withinDistanceSB.value
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

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
