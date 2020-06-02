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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.2

Item {
    id: targetPage

    property bool valid: attributeValueEdit.length > 0
    property string instruction: "Set attribute value"
    property alias attributeValue: attributeValueEdit.text
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    function text() {
        return attributeValueEdit.text;
    }

    function clear() {
        attributeValueEdit.text = "";
    }

    TextField {
        id: attributeValueEdit
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        color: Material.accent
        width: parent.width * 0.75
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            bold: true
        }

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        placeholderText: "<enter value>"
    }
}
