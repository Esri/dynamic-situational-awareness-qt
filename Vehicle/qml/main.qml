
// Copyright 2017 ESRI
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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0
import Esri.Vehicle 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Vehicle {
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    LocationController {
        id: locationController
        enabled: locationCheckBox.checked
    }        

    GenericToolbar {
        id: toolbar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        fontSize: 24 * scaleFactor
        toolbarLabelText: "DSA - Vehicle"

        onMenuClicked: {
            console.log("Menu button was clicked");
        }

        Row {
            spacing: 7 * scaleFactor
            anchors {
                right: parent.right
                margins: 2 * scaleFactor
                verticalCenter: parent.verticalCenter
            }

            Button {
                id: basemapsCheckBox
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: basemapsCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: basemapsCheckBox.background.height - (6 * scaleFactor)
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png"
                }

                onClicked: {
                    if (drawer.visible)
                        drawer.close();
                    else {
                        toolRect.state = "basemap";
                        drawer.open();
                    }
                }
            }

            Button {
                id: addLocalDataCheckBox
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: addLocalDataCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: addLocalDataCheckBox.background.height - (6 * scaleFactor)
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png"
                }

                onClicked: {
                    if (drawer.visible)
                        drawer.close();
                    else {
                        toolRect.state = "data";
                        drawer.open();
                    }
                }
            }

            Button {
                id: messageFeedsCheckBox
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: messageFeedsCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: messageFeedsCheckBox.background.height - (6 * scaleFactor)
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_messages_dark.png"
                }

                onClicked: {
                    if (drawer.visible)
                        drawer.close();
                    else {
                        toolRect.state = "message";
                        drawer.open();
                    }
                }
            }

            Button {
                id: tocCheckBox
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: tocCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: tocCheckBox.background.height - (6 * scaleFactor)
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_openlistview_dark.png"
                }

                onClicked: {
                    tableOfContentsTool.visible = !tableOfContentsTool.visible;
                }
            }

            Button {
                id: coordConvCheckBox
                checkable: true
                checked: false
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: coordConvCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/icon-64-coorconv-white.png"
                }
            }
			
            Button {
                id: analysisCheckBox
                checkable: true
                checked: false
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: analysisCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_video_dark_d.png"
                }

                onClicked: {
                    if (drawer.visible)
                        drawer.close();
                    else {
                        toolRect.state = "analysis";
                        drawer.open();
                    }
                }
            }

            Button {
                id: navCheckBox
                checkable: true
                checked: true
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: navCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: navCheckBox.checked ? "qrc:/Resources/icons/xhdpi/ic_menu_gpson_dark.png" : "qrc:/Resources/icons/xhdpi/ic_menu_gpson_dark_d.png"
                }
            }

            Button {
                id: telestrateCheckBox
                checkable: true
                checked: false
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: telestrateCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: telestrateTool.visible ? "qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchon_dark.png" :
                                                     "qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png"
                }

                onClicked: {
                    telestrateTool.visible = !telestrateTool.visible;
                }
            }

            Button {
                id: optionsCheckBox
                checkable: true
                checked: false
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: optionsCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png"
                }

                onClicked: {
                    if (drawer.visible)
                        drawer.close();
                    else {
                        toolRect.state = "options";
                        drawer.open();
                    }
                }
            }

            Button {
                id: alertsCheckBox
                checkable: true
                checked: false
                width: 32 * scaleFactor
                height: 32 * scaleFactor

                background: Rectangle {
                    anchors.fill: alertsCheckBox
                    color: Material.primary
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    sourceSize.height: parent.height * 0.85
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_failedlayer.png"
                }

                onClicked: {
                    alertsTool.visible = !alertsTool.visible;
                }

                ViewedAlerts {
                    anchors {
                        left: alertsCheckBox.horizontalCenter
                        bottom: alertsCheckBox.verticalCenter
                    }
                }
            }
        }
    }

    CoordinateConversion {
        id: coordinateConversion
        objectName: "coordinateConversion"
        visible: coordConvCheckBox.checked
        height: parent.height / 2
        width: parent.width
        anchors {
            bottom: parent.bottom
        }
        color: Material.primary
        textColor: Material.foreground
    }

    // Create SceneQuickView here, and create its Scene etc. in C++ code
    SceneView {
        id: sceneView
        anchors {
            top: toolbar.bottom
            left: parent.left
            right: parent.right
            bottom: coordConvCheckBox.checked ? coordinateConversion.top : parent.bottom
        }

        objectName: "sceneView"

        onMousePressed: followHud.stopFollowing();

        TableOfContents {
            id: tableOfContentsTool
            anchors {
                left: parent.left
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false

            onClosed: visible = false;
        }

        AlertList {
            id: alertsTool
            anchors {
                left: parent.left
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false

            onClosed: visible = false;
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
            visible: navCheckBox.checked
            spacing: 1 * scaleFactor

            anchors {
                margins: 8 * scaleFactor
                bottom: sceneView.attributionTop
                right: sceneView.right
            }

            NavigationTool {
                id: navTool

                visible: compass.visible && navCheckBox.checked
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
