// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

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
        onToolSelected: appRoot.showClearDialog("Are you sure you want to clear all markups?")
    }
}
