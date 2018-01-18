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
import Esri.Vehicle 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Row {
    id: analysisToolRow
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "options"
    onVisibleChanged: state = "clear"

    states: [
        State {
            name: settingsIcon.toolName
            PropertyChanges {
                target: settingsIcon
                selected: true
            }
        },
        State {
            name: aboutIcon.toolName
            PropertyChanges {
                target: aboutIcon
                selected: true
            }
        },
        State {
            name: "clear"
            PropertyChanges {
                target: settingsIcon
                selected: false
            }
            PropertyChanges {
                target: aboutIcon
                selected: false
            }
            PropertyChanges {
                target: closeIcon
                selected: false
            }
        }
    ]

    // Settings
    ToolIcon {
        id: settingsIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png"
        toolName: "Settings"
        onToolSelected: {
            parent.state = toolName;
            optionsTool.visible = selected;
        }
    }

    // About
    ToolIcon {
        id: aboutIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_about_dark_d.png"
        toolName: "About"
        onToolSelected: {
            optionsTool.visible = false;
            parent.state = toolName;
            // TODO - not implemented yet
        }
    }

    // Close Tool
    ToolIcon {
        id: closeIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_multiply_dark_d.png"
        toolName: "Close"
        onToolSelected: Qt.quit()
    }
}
