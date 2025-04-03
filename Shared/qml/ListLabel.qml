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
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property alias text: label.text
    property alias separatorVisible: separator.visible
    signal triggered()

    width: parent.width
    height: 32 * scaleFactor

    Label {
        id: label
        anchors.centerIn: parent

        font {
            family: DsaStyles.fontFamily
            pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
        }
        color: Material.foreground
    }

    Rectangle {
        id: separator
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: 8 * scaleFactor
            rightMargin: 8 * scaleFactor
        }

        height: 1 * scaleFactor
        color: Material.foreground
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.triggered();
        }
    }
}
