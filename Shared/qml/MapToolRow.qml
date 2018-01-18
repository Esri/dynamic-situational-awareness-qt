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
    id: mapToolRow
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "map"
    onVisibleChanged: state = "clear"
    property alias tocIconSelected: tocIcon.selected

    states: [
        State {
            name: identifyIcon.toolName
            PropertyChanges {
                target: identifyIcon
                selected: true
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
            PropertyChanges {
                target: identifyIcon
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
                selected: selected
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
                selected: selected
            }
        }
    ]

    // Identify Tool
    ToolIcon {
        id: identifyIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_aboutmap_dark.png"
        toolName: "Identify"
        onToolSelected: {
            if (selected) {
                selected = false;
                mapToolRow.state = "clear";
            } else
                mapToolRow.state = toolName;

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
        iconSource: "qrc:/Resources/icons/xhdpi/icon-64-coorconv-white.png"
        toolName: "Convert"
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

    // Add Local Data Tool
    ToolIcon {
        id: messageFeedsIcon
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_messages_dark.png"
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
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_layergroup_dark_d.png"
        toolName: "Layers"
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
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png"
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
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png"
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
