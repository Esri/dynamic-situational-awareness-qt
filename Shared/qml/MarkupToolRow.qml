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
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.4

Row {
    id: markupToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    spacing: 5 * scaleFactor
    visible: categoryToolbar.state === "markup"
    onVisibleChanged: {
        if (visible) {
            state = drawIcon.toolName
        } else {
            state = "clear";
        }
    }

    property alias configureSelected: colorIcon.selected
    property alias drawSelected: drawIcon.selected

    states: [
        State {
            name: clearIcon.toolName
            PropertyChanges {
                target: clearIcon
                selected: true
            }
            PropertyChanges {
                target: markup
                state: markup.clearState
            }
        },
        State {
            name: drawIcon.toolName
            PropertyChanges {
                target: drawIcon
                selected: true
            }
            PropertyChanges {
                target: markup
                visible: true
            }
            PropertyChanges {
                target: markup
                state: markup.drawState
            }
        },
        State {
            name: colorIcon.toolName
            PropertyChanges {
                target: colorIcon
                selected: true
            }
            PropertyChanges {
                target: markup
                visible: true
            }
            PropertyChanges {
                target: markup
                state: markup.colorState
            }
        },
        State {
            name: widthIcon.toolName
            PropertyChanges {
                target: widthIcon
                selected: true
            }
            PropertyChanges {
                target: markup
                visible: true
            }
            PropertyChanges {
                target: markup
                state: markup.widthState
            }
        },
        State {
            name: "clear"
            PropertyChanges {
                target: clearIcon
                selected: false
            }
            PropertyChanges {
                target: drawIcon
                selected: false
            }
        }
    ]

    // Draw
    ToolIcon {
        id: drawIcon
        iconSource: DsaResources.iconSketch
        toolName: "Draw"
        onToolSelected: {
            if (markupToolRow.state === toolName) {
                selected = false;
                markup.markupEnabled = false;
                markupToolRow.state = "clear";
            } else {
                markupToolRow.state = toolName;
            }
        }
    }

    // Color
    ToolIcon {
        id: colorIcon
        iconSource: DsaResources.iconColorPalette
        toolName: "Color"
        onToolSelected: {
            if (markupToolRow.state === toolName) {
                selected = false;
                markupToolRow.state = drawIcon.toolName;
                drawSelected = true;
            } else {
                markup.markupEnabled = false;
                markupToolRow.state = toolName;
            }
        }
    }

    // Width
    ToolIcon {
        id: widthIcon
        iconSource: DsaResources.iconLineWidth
        toolName: "Width"
        visible: false
        onToolSelected: {
            if (markupToolRow.state === toolName) {
                selected = false;
                markupToolRow.state = drawIcon.toolName
                drawSelected = true;
            } else {
                markup.markupEnabled = false;
                markupToolRow.state = toolName;
            }
        }
    }

    // Clear
    ToolIcon {
        id: clearIcon
        iconSource: DsaResources.iconTrash
        toolName: "Clear All"
        onToolSelected: appRoot.showClearDialog("Are you sure you want to clear the current markup sketch?")
    }
}
