// Copyright 2018 ESRI
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

Row {
    id: analysisToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

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
