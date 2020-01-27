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
import QtQuick.Controls 1.4 as QtQuick1
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.0

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    width : parent.width
    height: DsaStyles.mainToolbarHeight * scaleFactor
    visible: false

    DropShadow {
        anchors.fill: fill
        horizontalOffset: -1 * scaleFactor
        verticalOffset: 1 * scaleFactor
        radius: 8 * scaleFactor
        smooth: true
        samples: 16
        color: "#80000000"
        source: fill
    }

    Rectangle {
        id: fill
        anchors.fill: parent
        color: Material.background
    }
}
