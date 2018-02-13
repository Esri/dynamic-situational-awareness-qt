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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0
import Esri.Vehicle 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.2
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Vehicle {
    id: appRoot
    width: 800 * scaleFactor
    height: 600 * scaleFactor

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property alias messageFeeds: messageFeedsTool
    signal clearDialogAccepted();
    signal closeDialogAccepted();
    signal inputDialogAccepted(var input, var index);

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
        height: DsaStyles.mainToolbarHeight * scaleFactor
        menuVisible: true

        MapToolRow {
            id: mapToolRow
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * scaleFactor
            }
        }

        ReportToolRow {
            id: reportToolRow
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * scaleFactor
            }
        }

        AnalysisToolRow {
            id: analysisToolRow
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * scaleFactor
            }
        }

        AlertToolRow {
            id: alertToolRow
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * scaleFactor
            }
        }

        MarkupToolRow {
            id: markupToolRow
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10 * scaleFactor
            }
        }
    }

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

        DistressButton {
            anchors {
                top: parent.top
                horizontalCenter: navTool.horizontalCenter
                topMargin: sceneView.height < navTool.height * 1.75 ? 10 * scaleFactor : 40 * scaleFactor
            }
            messageFeedsController: messageFeeds.controller
        }

        CurrentLocation {
            id: currentLocation
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 10 * scaleFactor
            }
        }

        Rectangle {
            anchors {
                fill: followHud
                margins: -5 * scaleFactor
            }
            visible: followHud.enabled
            color: Material.primary
            radius: 5 * scaleFactor
            opacity: 0.5
        }

        FollowHud {
            id: followHud
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: currentLocation.visible ? currentLocation.height + 25 * scaleFactor : 10 * scaleFactor
            }
            enabled: false
        }

        NavigationTool {
            id: navTool
            anchors {
                margins: 10 * scaleFactor
                verticalCenter: parent.verticalCenter
                right: sceneView.right
            }
        }

        ArcGISCompass {
            id: compass
            anchors {
                horizontalCenter: navTool.horizontalCenter
                bottom: sceneView.attributionTop
                margins: 10 * scaleFactor
            }
            autoHideCompass: false
            width: DsaStyles.primaryIconSize * scaleFactor
            height: width
        }

        CategoryToolbarColumn {
            id: categoryToolbar
            anchors {
                top: parent.top
                left: parent.left
                bottom: sceneView.attributionTop
            }
            width: 56 * scaleFactor
            appTitle: "DSA - V"
            opacity: 0.75

            onSettingsClicked: optionsTool.visible = true;
            onAboutClicked: aboutTool.visible = true;
        }

        TableOfContents {
            id: tableOfContentsTool
            anchors {
                right: parent.right
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false
            isMobile: false
            onClosed: {
                mapToolRow.tocIconSelected = false;
                visible = false;
                mapToolRow.state = "clear";
            }
        }

        AlertList {
            id: alertsTool
            anchors {
                right: parent.right
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false
            onClosed: visible = false;
        }

        AlertConditionsTool {
            id: alertConditionsTool
            anchors {
                right: parent.right
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false
            onClosed: visible = false;
        }

        MarkupTool {
            id: markup
            anchors {
                right: parent.right
                top: parent.top
            }
            visible: false
        }

        Analysis {
            id: analysisTool
            anchors {
                right: parent.right
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: drawer.width
            visible: false
            isMobile: false
            onClosed: {
                visible = false;
                analysisToolRow.state = "clear";
            }
        }

        PopupStackView {
            id: identifyResults
            anchors {
                right: parent.right
                top: sceneView.top
                bottom: sceneView.attributionTop
            }
            backgroundColor: Material.primary
            attributeNameTextColor: Material.foreground
            attributeValueTextColor: Material.foreground
            titleTextColor: Material.foreground
            closeButtonColor: Material.foreground
        }

        Drawer {
            id: drawer
            width: 272 * scaleFactor
            height: sceneView.height - 20 * scaleFactor // approximation for attribution text
            edge: Qt.RightEdge
            y: topToolbar.height
            interactive: x < appRoot.width

            onClosed: {
                // update state for each category
                mapToolRow.state = "clear";
                alertToolRow.state = "clear";
                analysisTool.state = "clear";
                reportToolRow.state = "clear";
                markupToolRow.state = "clear";
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
            }
        }
    }

    CoordinateConversion {
        id: coordinateConversion
        anchors.bottom: parent.bottom
        objectName: "coordinateConversion"
        visible: false
        height: parent.height / 2
        width: parent.width
        color: Material.primary
        textColor: Material.foreground
    }

    About {
        id: aboutTool
        anchors.fill: parent
        visible: false
    }

    IdentifyController {
        id: identifyController
        active: mapToolRow.state === "Identify"

        onActiveChanged: {
            if (!active)
                identifyResults.dismiss();
        }

        onPopupManagersChanged: {
            identifyResults.dismiss();
            identifyResults.popupManagers = popupManagers;

            if (popupManagers.length > 0)
                identifyResults.show();
        }
    }

    onErrorOccurred: {
        msgDialog.informativeText = message;
        msgDialog.open();
    }

    Options {
        id: optionsTool
        anchors.fill: parent
        visible: false
    }

    DsaMessageDialog {
        id: msgDialog
        title: "Error"
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: identifyController.busy
    }

    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    DsaMessageDialog {
        id: clearDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: clearDialogAccepted();
    }

    function showClearDialog(title) {
        clearDialog.informativeText = title;
        clearDialog.open();
    }

    DsaMessageDialog {
        id: closeDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: closeDialogAccepted();
    }

    function showCloseDialog(title) {
        closeDialog.informativeText = title;
        closeDialog.open();
    }

    DsaInputDialog {
        id: inputDialog
        property int i: 1
        onAccepted: {
            i++;
            inputDialogAccepted(inputDialog.userInputText, i);
        }
    }

    function showInputDialog(labelText, placeholderText) {
        inputDialog.inputLabel = labelText;
        inputDialog.inputPlaceholderText = placeholderText;
        inputDialog.open();
    }
}
