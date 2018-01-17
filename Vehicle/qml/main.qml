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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0
import Esri.Vehicle 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Vehicle {
    id: vehicleRoot
    width: 800 * scaleFactor
    height: 600 * scaleFactor

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    LocationController {
        id: locationController
        enabled: true
    }

    PrimaryToolbar {
        id: topToolbar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        toolbarLabelText: categoryToolbar.titleText


        Row {
            id: mapToolRow
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * scaleFactor
            }
            spacing: 10 * scaleFactor
            visible: categoryToolbar.state === "map"

            states: [
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
                },
                State {
                    name: tocIcon.toolName
                    PropertyChanges {
                        target: tocIcon
                        selected: true
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
                },
                State {
                    name: "clear"
                    PropertyChanges {
                        target: coordinateConversionIcon
                        selected: false
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

                    if (tableOfContentsTool.visible) {
                        tableOfContentsTool.visible = false;
                        mapToolRow.state = "clear";
                        selected = false;
                    } /*else {
                        //coord.visible = true;
                    }*/
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
    }

    //            Button {
    //                id: analysisCheckBox
    //                checkable: true
    //                checked: false
    //                width: 32 * scaleFactor
    //                height: 32 * scaleFactor

    //                background: Rectangle {
    //                    anchors.fill: analysisCheckBox
    //                    color: Material.primary
    //                }

    //                Image {
    //                    fillMode: Image.PreserveAspectFit
    //                    anchors.centerIn: parent
    //                    sourceSize.height: parent.height * 0.85
    //                    height: sourceSize.height
    //                    source: "qrc:/Resources/icons/xhdpi/ic_menu_video_dark_d.png"
    //                }

    //                onClicked: {
    //                    if (drawer.visible)
    //                        drawer.close();
    //                    else {
    //                        toolRect.state = "analysis";
    //                        drawer.open();
    //                    }
    //                }
    //            }

    //            Button {
    //                id: navCheckBox
    //                checkable: true
    //                checked: true
    //                width: 32 * scaleFactor
    //                height: 32 * scaleFactor

    //                background: Rectangle {
    //                    anchors.fill: navCheckBox
    //                    color: Material.primary
    //                }

    //                Image {
    //                    fillMode: Image.PreserveAspectFit
    //                    anchors.centerIn: parent
    //                    sourceSize.height: parent.height * 0.85
    //                    height: sourceSize.height
    //                    source: navCheckBox.checked ? "qrc:/Resources/icons/xhdpi/ic_menu_gpson_dark.png" : "qrc:/Resources/icons/xhdpi/ic_menu_gpson_dark_d.png"
    //                }
    //            }

    //            Button {
    //                id: telestrateCheckBox
    //                checkable: true
    //                checked: false
    //                width: 32 * scaleFactor
    //                height: 32 * scaleFactor

    //                background: Rectangle {
    //                    anchors.fill: telestrateCheckBox
    //                    color: Material.primary
    //                }

    //                Image {
    //                    fillMode: Image.PreserveAspectFit
    //                    anchors.centerIn: parent
    //                    sourceSize.height: parent.height * 0.85
    //                    height: sourceSize.height
    //                    source: telestrateTool.visible ? "qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchon_dark.png" :
    //                                                     "qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png"
    //                }

    //                onClicked: {
    //                    telestrateTool.visible = !telestrateTool.visible;
    //                }
    //            }

    //            Button {
    //                id: optionsCheckBox
    //                checkable: true
    //                checked: false
    //                width: 32 * scaleFactor
    //                height: 32 * scaleFactor

    //                background: Rectangle {
    //                    anchors.fill: optionsCheckBox
    //                    color: Material.primary
    //                }

    //                Image {
    //                    fillMode: Image.PreserveAspectFit
    //                    anchors.centerIn: parent
    //                    sourceSize.height: parent.height * 0.85
    //                    height: sourceSize.height
    //                    source: "qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png"
    //                }

    //                onClicked: {
    //                    if (drawer.visible)
    //                        drawer.close();
    //                    else {
    //                        toolRect.state = "options";
    //                        drawer.open();
    //                    }
    //                }
    //            }
    //        }
    // }

    // Create SceneQuickView here, and create its Scene etc. in C++ code
    SceneView {
        id: sceneView
        anchors {
            top: topToolbar.bottom
            left: parent.left
            right: parent.right
            bottom: coordinateConversion.visible ? coordinateConversion.top : parent.bottom
        }
        objectName: "sceneView"

        onMousePressed: followHud.stopFollowing();

        FollowHud {
            id: followHud
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 8 * scaleFactor
            }

            enabled: locationCheckBox.checked
        }

        Column {
            id: navToolsColumn
            //visible: navCheckBox.checked
            spacing: 1 * scaleFactor

            anchors {
                margins: 8 * scaleFactor
                bottom: sceneView.attributionTop
                right: sceneView.right
            }

            NavigationTool {
                id: navTool

                //visible: compass.visible && navCheckBox.checked
            }

            Button {
                id: locationCheckBox
                checkable: true
                checked: false
                width: 32 * scaleFactor
                height: 32 * scaleFactor
                opacity: 0.9

                background: Rectangle {
                    anchors.fill: locationCheckBox
                    color: Material.primary
                    radius: 5 * scaleFactor
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: locationCheckBox.checked ?
                                "qrc:/Resources/icons/xhdpi/ic_menu_gpson_dark.png" :
                                "qrc:/Resources/icons/xhdpi/ic_menu_gpsondontfollow_dark.png"

                }
            }

            ArcGISCompass {
                id: compass

                autoHideCompass: false
            }
        }

        CategoryToolbar {
            id: categoryToolbar
            anchors {
                top: parent.top
                left: parent.left
                bottom: sceneView.attributionTop
            }
            width: 56 * scaleFactor
        }

        TableOfContents {
            id: tableOfContentsTool
            anchors {
                left: parent.left
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false

            onClosed: {
                tocIcon.selected = false;
                visible = false;
            }
        }

        TelestrateTool {
            id: telestrateTool
            anchors {
                left: parent.left
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false

            onClosed: visible = false;
        }

        Drawer {
            id: drawer
            width: 272 * scaleFactor
            height: parent.height

            onClosed: {
                // update toggle selected state for the map tool row
                mapToolRow.state = "clear";
            }

            Rectangle {
                id: toolRect
                anchors.fill: parent

                states: [
                    State {
                        name: "basemap"
                        PropertyChanges {
                            target: basemapsTool
                            visible: true
                        }
                    },
                    State {
                        name: "data"
                        PropertyChanges {
                            target: addLocalDataTool
                            visible: true
                        }
                    },
                    State {
                        name: "message"
                        PropertyChanges {
                            target: messageFeedsTool
                            visible: true
                        }
                    },
                    State {
                        name: "analysis"
                        PropertyChanges {
                            target: analysisTool
                            visible: true
                        }
                    },
                    State {
                        name: "options"
                        PropertyChanges {
                            target: optionsTool
                            visible: true
                        }
                    }
                ]

                BasemapPicker {
                    id: basemapsTool
                    anchors.fill: parent
                    onBasemapSelected: closed();
                    visible: false
                    onClosed: drawer.close();
                }

                AddLocalData {
                    id: addLocalDataTool
                    anchors.fill: parent
                    showDataConnectionPane: true
                    visible: false
                    onClosed: drawer.close();
                }

                MessageFeeds {
                    id: messageFeedsTool
                    anchors.fill: parent
                    visible: false
                    onClosed: drawer.close();
                }

                Analysis {
                    id: analysisTool
                    anchors.fill: parent
                    visible: false
                    onClosed: drawer.close();
                }

                Options {
                    id: optionsTool
                    anchors.fill: parent
                    visible: false
                    onClosed: drawer.close();
                }
            }
        }
    }

    CoordinateConversion {
        id: coordinateConversion
        anchors.bottom: parent.bottom
        objectName: "coordinateConversion"
        visible: coordinateConversionIcon.selected
        height: parent.height / 2
        width: parent.width
        color: Material.primary
        textColor: Material.foreground
    }

    onErrorOccurred: {
        msgDialog.informativeText = message;
        msgDialog.detailedText = additionalMessage;
        msgDialog.open();
    }

    MessageDialog {
        id: msgDialog
        text: "Error"
    }
}
