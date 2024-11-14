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

Row {
    id: packagesToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "packages"
    onVisibleChanged: state = "clear"

    property alias observationState: packagesIcon.toolName
    property string clearState: "clear"

    states: [
        State {
            name: packagesIcon.toolName
            PropertyChanges {
                target: packagesIcon
                selected: true
            }
        },

        State {
            name: clearState
            PropertyChanges {
                target: packagesIcon
                selected: false
            }
        }
    ]

    // Packages Tool
    ToolIcon {
        id: packagesIcon
        iconSource: DsaResources.iconAddLayer
        toolName: "Packages"
        onToolSelected: {
            if (selected) {
                packagesToolRow.state = clearState;
                selected = false;
            }
            else
                packagesToolRow.state = toolName;
        }

        onSelectedChanged: {
            packagesTool.visible = selected;
        }
    }
}
