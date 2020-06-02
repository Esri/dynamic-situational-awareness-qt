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

Rectangle {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    width: toolController.unviewedCount > 0 ?
               (toolController.unviewedCount > 9 ? 16 * scaleFactor
                                                 : 12 * scaleFactor)
             : 0
    height: width
    color: "red"
    radius: width * 0.5

    ViewedAlertsController {
        id: toolController
    }

    Text {
        visible: parent.width > 0
        anchors.fill: parent
        color: "white"
        text: toolController.unviewedCount > 99 ? "N" : toolController.unviewedCount
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            bold: true
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
