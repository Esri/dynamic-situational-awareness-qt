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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    id: namePage

    property bool nameAlreadyInUse: toolController.conditionAlreadyAdded(newAlertName.text)
    property bool valid: newAlertName.length > 0 && !nameAlreadyInUse
    property string instruction: "Set name"
    property alias conditionName: newAlertName.text
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    function text() {
        return " called " + newAlertName.text;
    }

    function clear() {
        newAlertName.text = "";
    }

    TextField {
        id: newAlertName
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        color: Material.accent
        font {
            pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
            bold: true
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        placeholderText: "<enter name>"
    }
    Label {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: newAlertName.bottom
            margins: 16 * scaleFactor
        }
        font {
            pixelSize: DsaStyles.titleFontPixelSize * scaleFactor * 0.5
            italic: true
        }

        color: "red"
        opacity: nameAlreadyInUse ? 1.0 : 0.0
        text: "Name already in use"
    }
}
