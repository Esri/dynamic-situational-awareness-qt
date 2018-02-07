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
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Row {
    id: markupToolRow
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "markup"
    onVisibleChanged: {
        if (visible) {
            state = drawIcon.toolName
        } else {
            state = "clear";
        }
    }
    property alias configureSelected: colorIcon.selected

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
                state: markup.editState
            }
            PropertyChanges {
                target: drawIcon
                selected: selected
            }
        },
        State {
            name: markupTocIcon.toolName
            PropertyChanges {
                target: markupTocIcon
                selected: true
            }
            PropertyChanges {
                target: markup
                visible: true
            }
            PropertyChanges {
                target: markup
                state: markup.tocState
            }
            PropertyChanges {
                target: drawIcon
                selected: selected
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
                selected: selected
            }
            PropertyChanges {
                target: markupTocIcon
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
        iconSource: DsaResources.iconSettings
        toolName: "Color"
        onToolSelected: {
            if (markupToolRow.state === toolName) {
                selected = false;
                markupToolRow.state = "clear";
            } else {
                markupToolRow.state = toolName;
            }
        }
    }

    // Markups TOC
    ToolIcon {
        id: markupTocIcon
        iconSource: DsaResources.iconListView
        toolName: "Markups"
        onToolSelected: {
            if (markupToolRow.state === toolName)
                markupToolRow.state = "clear";
            else {
                markupToolRow.state = toolName;
            }
        }
    }

    // Clear
    ToolIcon {
        id: clearIcon
        iconSource: DsaResources.iconTrash
        toolName: "Clear All"
        onToolSelected: markup.clear();
    }
}
