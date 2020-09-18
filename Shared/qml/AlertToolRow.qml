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
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.5

Row {
    id: alertToolRow
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "alerts"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    onVisibleChanged: {
        if (visible) {
            viewAlertIcon.selected = true;
            alertToolRow.state = viewAlertIcon.toolName;
            alertsTool.visible = true;
        } else {
            state = "clear";
        }
    }

    states: [
        State {
            name: viewAlertIcon.toolName
            PropertyChanges {
                target: viewAlertIcon
                selected: true
            }
        },
        State {
            name: createAlertIcon.toolName
            PropertyChanges {
                target: createAlertIcon
                selected: true
            }
        },
        State {
            name: "clear"
            PropertyChanges {
                target: viewAlertIcon
                selected: false
            }
            PropertyChanges {
                target: createAlertIcon
                selected: false
            }
            PropertyChanges {
                target: alertConditionsTool
                visible: false
            }
            PropertyChanges {
                target: alertsTool
                visible: false
            }
        }
    ]

    // View Alert Tool
    ToolIcon {
        id: viewAlertIcon
        iconSource: DsaResources.iconAlertList
        toolName: "View"
        onToolSelected: {
            alertConditionsTool.visible = false;
            if (alertToolRow.state === toolName) {
                alertToolRow.state = "clear";
                alertsTool.visible = false;
            } else {
                alertToolRow.state = toolName;
                alertsTool.visible = true;
            }
        }
    }

    // Create Alert Tool
    ToolIcon {
        id: createAlertIcon
        iconSource: DsaResources.iconAlertConditionList
        toolName: "Conditions"
        onToolSelected: {
            alertsTool.visible = false;
            if (alertToolRow.state === toolName) {
                alertToolRow.state = "clear";
                alertConditionsTool.visible = false;
            } else {
                alertToolRow.state = toolName;
                alertConditionsTool.visible = true;
            }
        }
    }
}
