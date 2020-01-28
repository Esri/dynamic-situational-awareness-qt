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

Row {
    id: analysisToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    spacing: 18 * scaleFactor
    visible: categoryToolbar.state === "analysis"
    onVisibleChanged: analysisToolRow.state = "clear"

    states: [
        State {
            name: viewshedIcon.toolName
            PropertyChanges {
                target: viewshedIcon
                selected: true
            }
            PropertyChanges {
                target: lineOfSightIcon
                selected: false
            }
            PropertyChanges {
                target: analysisListIcon
                selected: false
            }
        },
        State {
            name: lineOfSightIcon.toolName
            PropertyChanges {
                target: lineOfSightIcon
                selected: true
            }
            PropertyChanges {
                target: viewshedIcon
                selected: false
            }
            PropertyChanges {
                target: analysisListIcon
                selected: false
            }
        },
        State {
            name: analysisListIcon.toolName
            PropertyChanges {
                target: analysisListIcon
                selected: true
            }
            PropertyChanges {
                target: viewshedIcon
                selected: false
            }
            PropertyChanges {
                target: lineOfSightIcon
                selected: false
            }
        },
        State {
            name: "clear"
            PropertyChanges {
                target: viewshedIcon
                selected: false
            }
            PropertyChanges {
                target: lineOfSightIcon
                selected: false
            }
            PropertyChanges {
                target: analysisListIcon
                selected: false
            }
        }
    ]

    // Viewshed Tool
    ToolIcon {
        id: viewshedIcon
        iconSource: DsaResources.iconViewshed
        toolName: "Viewshed"

        onToolSelected: {
            if (selected ) {
                analysisToolRow.state = "clear"
                selected = false;
            }
            else {
                analysisToolRow.state = toolName;
            }
        }

        onSelectedChanged: {
            viewshedTool.visible = selected;
        }
    }

    // Line of sight Tool
    ToolIcon {
        id: lineOfSightIcon
        iconSource: DsaResources.iconLineOfSight
        toolName: "Line of Sight"
        onToolSelected: {
            if (selected ) {
                analysisToolRow.state = "clear"
                selected = false;
            }
            else {
                analysisToolRow.state = toolName;
            }
        }

        onSelectedChanged: {
            lineOfSightTool.visible = selected;
        }
    }

    // Analysis List Tool
    ToolIcon {
        id: analysisListIcon
        iconSource: DsaResources.iconListView
        toolName: "View List"
        onToolSelected: {
            if (selected ) {
                analysisToolRow.state = "clear"
                selected = false;
            }
            else {
                analysisToolRow.state = toolName;
            }
        }

        onSelectedChanged: {
            analysisListTool.visible = selected;
        }
    }
}
