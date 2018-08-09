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
import Esri.DSA 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.3

Row {
    id: mapToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "map"
    onVisibleChanged: {
        tableOfContentsTool.visible = false;
        state = "clear";
    }
    property alias tocIconSelected: tocIcon.selected

    states: [
        State {
            name: identifyIcon.toolName
            PropertyChanges {
                target: identifyIcon
                selected: true
            }
            PropertyChanges {
                target: tocIcon
                selected: selected
            }
        },
        State {
            name: coordinateConversionIcon.toolName
            PropertyChanges {
                target: coordinateConversionIcon
                selected: true
            }
            PropertyChanges {
                target: tocIcon
                selected: selected
            }
        },
        State {
            name: messageFeedsIcon.toolName
            PropertyChanges {
                target: messageFeedsIcon
                selected: true
            }
            PropertyChanges {
                target: tocIcon
                selected: selected
            }
            PropertyChanges {
                target: coordinateConversionIcon
                selected: selected
            }
            PropertyChanges {
                target: identifyIcon
                selected: selected
            }
        },
        State {
            name: addLocalDataIcon.toolName
            PropertyChanges {
                target: addLocalDataIcon
                selected: true
            }
            PropertyChanges {
                target: tocIcon
                selected: selected
            }
            PropertyChanges {
                target: coordinateConversionIcon
                selected: selected
            }
            PropertyChanges {
                target: identifyIcon
                selected: selected
            }
        },
        State {
            name: tocIcon.toolName
            PropertyChanges {
                target: tocIcon
                selected: true
            }
            PropertyChanges {
                target: coordinateConversionIcon
                selected: selected
            }
            PropertyChanges {
                target: identifyIcon
                selected: selected
            }
        },
        State {
            name: basemapIcon.toolName
            PropertyChanges {
                target: basemapIcon
                selected: true
            }
            PropertyChanges {
                target: tocIcon
                selected: selected
            }
            PropertyChanges {
                target: coordinateConversionIcon
                selected: selected
            }
            PropertyChanges {
                target: identifyIcon
                selected: selected
            }
        },
        State {
            name: "clear"
            PropertyChanges {
                target: identifyIcon
                selected: false
            }
            PropertyChanges {
                target: coordinateConversionIcon
                selected: selected
            }
            PropertyChanges {
                target: messageFeedsIcon
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
                target: tocIcon
                selected: false
            }
        }
    ]

    // Identify Tool
    ToolIcon {
        id: identifyIcon
        iconSource: DsaResources.iconAboutMap
        toolName: "Query"
        visible: false
        onToolSelected: {
            if (selected) {
                selected = false;
                mapToolRow.state = "clear";
            } else
                mapToolRow.state = toolName;

            identifyController.active = selected;

            if (identifyResults.visible) {
                identifyResults.visible = false;
                mapToolRow.state = "clear";
                selected = false;
            }
        }
    }

    // Coordinate Conversion Tool
    ToolIcon {
        id: coordinateConversionIcon
        iconSource: DsaResources.iconCoordinateConversion
        toolName: "Convert XY"
        onToolSelected: {
            if (selected)
                mapToolRow.state = "clear"
            else
                mapToolRow.state = toolName;

            if (coordinateConversion.visible) {
                coordinateConversion.visible = false;
                mapToolRow.state = "clear";
                selected = false;
            } else {
                coordinateConversion.visible = true;
            }
        }
    }

    // Message Feeds
    ToolIcon {
        id: messageFeedsIcon
        iconSource: DsaResources.iconMessages
        toolName: "Feeds"
        onToolSelected: {
            if (drawer.visible)
                drawer.close();
            else {
                mapToolRow.state = toolName;
                toolRect.state = "message";
                drawer.open();
            }
        }
    }

    // Table of Contents
    ToolIcon {
        id: tocIcon
        iconSource: DsaResources.iconLayers
        toolName: "Overlays"
        onToolSelected: {
            if (selected)
                mapToolRow.state = "clear"
            else
                mapToolRow.state = toolName;

            if (tableOfContentsTool.visible) {
                tableOfContentsTool.visible = false;
                mapToolRow.state = "clear";
                selected = false;
            } else {
                tableOfContentsTool.visible = true;
            }
        }
    }

    // Add Local Data Tool
    ToolIcon {
        id: addLocalDataIcon
        iconSource: DsaResources.iconAddLayer
        toolName: "Add Data"
        onToolSelected: {
            if (drawer.visible)
                drawer.close();
            else {
                mapToolRow.state = toolName;
                toolRect.state = "data";
                drawer.open();
            }
        }
    }

    // Basemap Tool
    ToolIcon {
        id: basemapIcon
        iconSource: DsaResources.iconChooseBasemap
        toolName: "Basemap"
        onToolSelected: {
            if (drawer.visible)
                drawer.close();
            else {
                toolRect.state = "basemap";
                mapToolRow.state = toolName;
                drawer.open();
            }
        }
    }
}
