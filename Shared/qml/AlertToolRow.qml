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
    id: alertToolRow
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "alerts"
    onVisibleChanged: {
        state = "clear"
        if (visible)
        {
            viewAlertIcon.selected = true;
            alertToolRow.state = viewAlertIcon.toolName;
            alertsTool.visible = true;
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
        }
    ]

    // View Alert Tool
    ToolIcon {
        id: viewAlertIcon
        iconSource: DsaResources.iconListView
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
        iconSource: DsaResources.iconDraw
        toolName: "Conditions"
        onToolSelected: {
            identifyController.active = false;
            identifyIcon.selected = false;
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
