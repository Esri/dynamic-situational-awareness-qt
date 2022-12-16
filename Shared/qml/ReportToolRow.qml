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
    id: reportToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "reports"
    onVisibleChanged: state = "clear"

    property alias observationState: observationIcon.toolName
    property string clearState: "clear"

    states: [
        State {
            name: observationIcon.toolName
            PropertyChanges {
                target: observationIcon
                selected: true
            }
        },

        State {
            name: clearState
            PropertyChanges {
                target: observationIcon
                selected: false
            }
        }
    ]

    // Observation Tool
    ToolIcon {
        id: observationIcon
        iconSource: DsaResources.iconCreateReport
        toolName: "Observation"
        onToolSelected: {
            if (selected) {
                reportToolRow.state = clearState;
                selected = false;
            }
            else
                reportToolRow.state = toolName;
        }

        onSelectedChanged: {
            observationReportTool.visible = selected;
        }
    }
}
