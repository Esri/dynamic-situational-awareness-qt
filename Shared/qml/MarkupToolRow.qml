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
    onVisibleChanged: state = "clear"

    states: [
        State {
            name: clearIcon.toolName
            PropertyChanges {
                target: clearIcon
                selected: true
            }
        },
        State {
            name: drawIcon.toolName
            PropertyChanges {
                target: drawIcon
                selected: true
            }
            PropertyChanges {
                target: telestrateTool
                visible: true
            }
        },
        State {
            name: configureIcon.toolName
            PropertyChanges {
                target: configureIcon
                selected: true
            }
        },
        State {
            name: sendIcon.toolName
            PropertyChanges {
                target: sendIcon
                selected: true
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
            PropertyChanges {
                target: sendIcon
                selected: false
            }
        }
    ]

    // Send
    ToolIcon {
        id: sendIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png"
        toolName: "Send"
        onToolSelected: {
            // TODO
        }
    }

    // Draw
    ToolIcon {
        id: drawIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png"
        toolName: "Draw"
        onToolSelected: {
            if (markupToolRow.state === toolName)
                markupToolRow.state = "clear";
            else {
                markupToolRow.state = toolName;
            }
        }
    }

    // Configure
    ToolIcon {
        id: configureIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png"
        toolName: "Configure"
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
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_trash_dark_d.png"
        toolName: "Clear"
        onToolSelected: {
            // TODO - clear graphics overlay
        }
    }
}
