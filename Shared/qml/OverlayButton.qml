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

Rectangle {
    id: overlayButton
    property alias iconUrl: buttonIcon.source
    property bool selected: false
    property string name: ""
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    signal clicked()

    width: DsaStyles.secondaryIconSize * scaleFactor
    height: width
    color: Material.primary
    radius: 5 * scaleFactor
    opacity: 0.9

    Image {
        id: buttonIcon
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: width
    }

    Rectangle {
        anchors {
            topMargin: 2 * scaleFactor
            horizontalCenter: parent.horizontalCenter
            top: buttonIcon.bottom
        }
        visible: selected
        height: 2 * scaleFactor
        width: parent.width
        color: Material.accent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: overlayButton.clicked()
    }
}
