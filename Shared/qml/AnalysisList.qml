
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
import QtQml.Models 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    id: analysisRoot
    title: "Analysis List"
    clip: true

    property bool isMobile

    // Create the controller
    AnalysisListController {
        id: toolController
    }

    ListView {
        id: analysisList
        anchors {
            top: analysisRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        interactive: true
        clip: true
        spacing: 5 * scaleFactor
        model: toolController.analysisList
        highlightFollowsCurrentItem: isMobile
        highlight: Rectangle {
            radius: 5 * scaleFactor
            color: Material.accent
            opacity: 0.5
        }
        delegate: ListItemDelegate {
            width: parent.width
            height: 40 * scaleFactor
            itemChecked: analysisVisible
            imageUrl: imageSourceForAnalysisType(analysisType)
            imageVisible: true
            imageFrameVisible: false
            menuIconVisible: true
            mainText: analysisName
            onItemCheckedChanged: {
                if (analysisVisible !== itemChecked)
                    analysisVisible = itemChecked
            }

            Image {
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: 5 * scaleFactor
                }
                rotation: 90
                source: DsaResources.iconMenu
                height: 32 * scaleFactor
                width: height

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        analysisList.currentIndex = index;
                        if (!isMobile) {
                            vehicleMenu.open();
                        } else {
                            if (mobileMenu.isOpen) {
                                mobileMenu.close();
                            } else {
                                mobileMenu.open();
                            }
                        }
                    }
                }

                // Menu for Vehicle
                Menu {
                    id: vehicleMenu
                    width: 125 * scaleFactor

                    Column {
                        anchors.margins: 10 * scaleFactor
                        width: parent.width
                        spacing: 10 * scaleFactor

                        ListLabel {
                            text: "Zoom to"
                            onTriggered: {
                                vehicleMenu.close();
                                toolController.zoomTo(analysisList.currentIndex);
                            }
                        }

                        ListLabel {
                            text: "Remove"
                            separatorVisible: false
                            onTriggered: {
                                vehicleMenu.close()
                                toolController.removeAt(analysisList.currentIndex);
                            }
                        }
                    }
                }
            }

            function imageSourceForAnalysisType(analysisType){

                if (analysisType === "lineOfSight") {
                    return DsaResources.iconLineOfSight;
                } else if (analysisType === "viewshed") {
                    return DsaResources.iconViewshed;
                }

                return "";
            }
        }
    }

    Rectangle {
        id: mobileMenu
        property bool isOpen: y === analysisRoot.y + analysisRoot.height - height
        property int closedY: analysisRoot.y + analysisRoot.height
        property int openY: analysisRoot.y + analysisRoot.height - height - anchors.margins
        anchors {
            left: parent.left
            right: parent.right
            margins: 5 * scaleFactor
        }
        color: "transparent"
        height: analysisRoot.height
        y: closedY

        MouseArea {
            anchors.fill: parent
            onClicked: mobileMenu.close()
        }

        Rectangle {
            anchors {
                fill: mobileActionColumn
                margins: -10 * scaleFactor
            }
            color: Material.background
            radius: 10 * scaleFactor
            border {
                color: Material.primary
                width: 1 * scaleFactor
            }
        }

        function open() {
            if (y === openY)
                return;

            analysisList.highlightFollowsCurrentItem = true;
            animateVertical.from = closedY;
            animateVertical.to = openY;
            animateVertical.start();
        }

        function close() {
            if (y === closedY)
                return;

            analysisList.highlightFollowsCurrentItem = false;
            animateVertical.from = openY;
            animateVertical.to = closedY;
            animateVertical.start();
            analysisList.currentIndex = -1;
        }

        NumberAnimation {
            id: animateVertical
            target: mobileMenu
            properties: "y"
            duration: 250
            easing.type: Easing.OutQuad
        }

        Column {
            id: mobileActionColumn
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 10 * scaleFactor
            }

            spacing: 5 * scaleFactor

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Zoom to"
                onTriggered: {
                    var i = analysisList.currentIndex;
                    mobileMenu.close();
                    analysisRoot.closed();
                    toolController.zoomTo(i);
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Remove"
                onTriggered: {
                    toolController.removeAt(analysisList.currentIndex);
                    mobileMenu.close()
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                separatorVisible: false
                text: "Cancel"
                onTriggered: {
                    mobileMenu.close();
                }
            }
        }
    }

    Text {
        anchors {
            left: parent.left
            right: parent.right
            top: analysisRoot.titleBar.bottom
            margins: 15 * scaleFactor
        }
        visible: analysisList.count === 0
        text: "No analyses have been added.\n\nSelect 'Line of Sight' or 'Viewshed' \nto perform analysis."
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
    }
}
