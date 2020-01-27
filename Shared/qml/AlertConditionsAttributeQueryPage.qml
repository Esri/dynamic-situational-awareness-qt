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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.0

Item {
    id: queryPage

    property bool valid: attributeFieldEdit.length > 0
    property string instruction: "Select attribute"
    property alias attributeField: attributeFieldEdit.text
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

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
