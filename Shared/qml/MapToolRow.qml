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
    id: mapToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "map"
    onVisibleChanged: {
        messageFeedsTool.visible    = false;
        tableOfContentsTool.visible = false;
        addLocalDataTool.visible    = false;
        basemapsTool.visible        = false;
        openSceneTool.visible       = false;
        state = "clear";
    }

    states: [
        State {
            name: "clear"
            PropertyChanges {
                target: coordinateConversionIcon
                selected: selected
            }
            PropertyChanges {
                target: messageFeedsIcon
                selected: false
            }
            PropertyChanges {
                target: tocIcon
                selected: false
            }
            PropertyChanges {
                target: addLocalDataIcon
                selected: false
            }
            PropertyChanges {
                target: basemapIcon
                selected: false
            }
            PropertyChanges {
                target: packageIcon
                selected: false
            }
        }
    ]

    // Coordinate Conversion Tool
    ToolIcon {
        id: coordinateConversionIcon
        iconSource: DsaResources.iconCoordinateConversion
        toolName: "Convert XY"
        onToolSelected: {
            selected = !selected;
            dsaCoordinateController.active = selected;
        }
    }

    // Message Feeds
    ToolIcon {
        id: messageFeedsIcon
        iconSource: DsaResources.iconMessages
        toolName: "Feeds"
        onToolSelected: {
            messageFeedsTool.visible    = messageFeedsIcon.selected = !selected;
            tableOfContentsTool.visible = tocIcon.selected          = false;
            addLocalDataTool.visible    = addLocalDataIcon.selected = false;
            basemapsTool.visible        = basemapIcon.selected      = false;
            openSceneTool.visible       = packageIcon.selected      = false;
        }
    }

    // Table of Contents
    ToolIcon {
        id: tocIcon
        iconSource: DsaResources.iconLayers
        toolName: "Overlays"
        onToolSelected: {
            messageFeedsTool.visible    = messageFeedsIcon.selected = false;
            tableOfContentsTool.visible = tocIcon.selected          = !selected;
            addLocalDataTool.visible    = addLocalDataIcon.selected = false;
            basemapsTool.visible        = basemapIcon.selected      = false;
            openSceneTool.visible       = packageIcon.selected      = false;
        }
    }

    // Add Local Data Tool
    ToolIcon {
        id: addLocalDataIcon
        iconSource: DsaResources.iconAddLayer
        toolName: "Add Data"
        onToolSelected: {
            messageFeedsTool.visible    = messageFeedsIcon.selected = false;
            tableOfContentsTool.visible = tocIcon.selected          = false;
            addLocalDataTool.visible    = addLocalDataIcon.selected = !selected;
            basemapsTool.visible        = basemapIcon.selected      = false;
            openSceneTool.visible       = packageIcon.selected      = false;
        }
    }

    // Basemap Tool
    ToolIcon {
        id: basemapIcon
        iconSource: DsaResources.iconChooseBasemap
        toolName: "Basemap"
        onToolSelected: {
            messageFeedsTool.visible    = messageFeedsIcon.selected = false;
            tableOfContentsTool.visible = tocIcon.selected          = false;
            addLocalDataTool.visible    = addLocalDataIcon.selected = false;
            basemapsTool.visible        = basemapIcon.selected      = !selected;
            openSceneTool.visible       = packageIcon.selected      = false;
        }
    }

    // Open Scene Tool
    ToolIcon {
        id: packageIcon
        iconSource: DsaResources.iconOpenScene
        toolName: "Open"
        onToolSelected: {
            messageFeedsTool.visible    = messageFeedsIcon.selected = false;
            tableOfContentsTool.visible = tocIcon.selected          = false;
            addLocalDataTool.visible    = addLocalDataIcon.selected = false;
            basemapsTool.visible        = basemapIcon.selected      = false;
            openSceneTool.visible       = packageIcon.selected      = !selected;
        }
    }
}
