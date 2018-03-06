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

Row {
    id: analysisToolRow
    spacing: 16 * scaleFactor
    visible: categoryToolbar.state === "analysis"
    onVisibleChanged: state = "clear"

    states: [
        State {
            name: viewshedIcon.toolName
            PropertyChanges {
                target: viewshedIcon
                selected: true
            }
        },
        State {
            name: lineOfSightIcon.toolName
            PropertyChanges {
                target: lineOfSightIcon
                selected: true
            }
        },
        State {
            name: analysisListIcon.toolName
            PropertyChanges {
                target: analysisListIcon
                selected: true
            }
        },
        State {
            name: "clear"
            PropertyChanges {
                target: viewshedIcon
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
            if (selected)
                analysisToolRow.state = "clear"
            else
                analysisToolRow.state = toolName;

            if (viewshedTool.visible) {
                viewshedTool.visible = false;
                analysisToolRow.state = "clear";
                selected = false;
            } else {
                viewshedTool.visible = true;
                lineOfSightTool.visible = false;
                analysisListTool.visible = false;
            }
        }
    }

    // Line of sight Tool
    ToolIcon {
        id: lineOfSightIcon
        iconSource: DsaResources.iconLineOfSight
        toolName: "Line of sight"
        onToolSelected: {
            if (selected)
                analysisToolRow.state = "clear"
            else
                analysisToolRow.state = toolName;

            if (lineOfSightTool.visible) {
                lineOfSightTool.visible = false;
                lineOfSightTool.state = "clear";
                selected = false;
            } else {
                lineOfSightTool.visible = true;
                viewshedTool.visible = false;
                analysisListTool.visible = false;
            }
        }
    }

    // Analysis List Tool
    ToolIcon {
        id: analysisListIcon
        iconSource: DsaResources.iconListView
        toolName: "List"
        onToolSelected: {
            if (selected)
                analysisToolRow.state = "clear"
            else
                analysisToolRow.state = toolName;

            if (analysisListTool.visible) {
                analysisListTool.visible = false;
                analysisListTool.state = "clear";
                selected = false;
            } else {
                analysisListTool.visible = true;
                lineOfSightTool.visible = false;
                viewshedTool.visible = false;
            }
        }
    }
}
