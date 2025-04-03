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
import QtQuick.Layouts
import Esri.ArcGISRuntime.OpenSourceApps.DSA


Item {
    width: backgroundRecatangle.width
    height: backgroundRecatangle.height
    property alias radius: backgroundRecatangle.radius
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    // Declare Controller Class
    LocationTextController {
        id: locationTextController
    }

    // Fill Rectangle to display behind text
    Rectangle {
        id: backgroundRecatangle
        anchors {
            margins: -2 * scaleFactor
            fill: displayColumn
        }
        color: Material.primary
        opacity: parent.opacity
    }

    // Column that displays the location and elevation text
    Column {
        id: displayColumn
        anchors.centerIn: parent
        spacing: 1 * scaleFactor

        Text {
            id: locationText
            font {
                pixelSize: 11 * scaleFactor
                family: DsaStyles.fontFamily
            }
            text: locationTextController.currentLocationText
            color: Material.foreground
        }

        Text {
            id: elevationText
            text: locationTextController.currentElevationText
            font {
                pixelSize: 11 * scaleFactor
                family: DsaStyles.fontFamily
            }
            color: Material.foreground
        }
    }
}
