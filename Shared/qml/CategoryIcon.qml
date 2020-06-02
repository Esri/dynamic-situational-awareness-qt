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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.2
import QtQuick.Controls.Material 2.2

Item {
    width: DsaStyles.secondaryIconSize * scaleFactor
    height: width
    property alias iconSource: image.source
    property string categoryName
    property bool selected: false
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    signal categorySelected()

    Column {
        anchors.fill: parent
        spacing: 2 * scaleFactor

        Image {
            id: image
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.75
            height: width
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            height: 2 * scaleFactor
            width: parent.width
            color: Material.accent
            visible: selected
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: categorySelected()
    }
}
