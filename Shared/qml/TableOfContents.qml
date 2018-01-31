
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
import QtQml.Models 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    id: tocRoot
    title: "Overlays"
    clip: true

    property bool isMobile

    // Create the controller
    TableOfContentsController {
        id: toolController
    }

    // Declare the ListView, which will display the list of files
    ListView {
        id: layersList
        anchors {
            top: tocRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        interactive: true
        clip: true
        spacing: 5 * scaleFactor
        model: toolController.layerListModel
        highlight: Rectangle {
            width: 50
            height: 50
            color: "lightsteelblue"
            visible: mobileMenu.isOpen
        }
        delegate: ListItemDelegate {
            width: parent.width
            height: 40 * scaleFactor
            itemVisible: layerVisible
            imageUrl: imageSourceForGeomType(index)
            imageVisible: true
            mainText: name && name !== "" ?
                          name :
                          toolController.alternateName(index)
            onItemVisibleChanged: layerVisible = itemVisible

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
                        layersList.currentIndex = index;
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
                Popup {
                    id: vehicleMenu

                    Column {
                        anchors.margins: 10 * scaleFactor
                        spacing: 10 * scaleFactor

                        ListLabel {
                            text: "Zoom to"
                            onTriggered: {
                                vehicleMenu.close();
                                tocRoot.closed();
                                toolController.zoomTo(layersList.currentIndex);
                            }
                        }

                        ListLabel {
                            text: "Remove"
                            onTriggered: {
                                vehicleMenu.close()
                                toolController.removeAt(layersList.currentIndex);
                            }
                        }

                        ListLabel {
                            text: "Move up"
                            visible: layersList.currentIndex !== 0
                            onTriggered: {
                                vehicleMenu.close()
                                toolController.moveUp(layersList.currentIndex);
                            }
                        }

                        ListLabel {
                            text: "Move down"
                            visible: layersList.currentIndex + 1 !== layersList.count
                            onTriggered: {
                                vehicleMenu.close()
                                toolController.moveDown(layersList.currentIndex);
                            }
                        }
                    }
                }
            }

            function imageSourceForGeomType(i){
                var geomType = toolController.layerGeometryType(i);
                switch (geomType) {
                case TableOfContentsController.Unknown:
                    return "";
                case TableOfContentsController.Points:
                    return DsaResources.iconPoint;
                case TableOfContentsController.Polylines:
                    return DsaResources.iconPolyline;
                case TableOfContentsController.Polygons:
                    return DsaResources.iconPolygon;
                case TableOfContentsController.Raster:
                    return DsaResources.iconRaster;
                }
            }
        }
    }

    Rectangle {
        id: mobileMenu
        property bool isOpen: y === tocRoot.y + tocRoot.height - height
        property int closedY: tocRoot.y + tocRoot.height
        property int openY: tocRoot.y + tocRoot.height - height - anchors.margins
        anchors {
            left: parent.left
            right: parent.right
            margins: 5 * scaleFactor
        }
        color: "transparent"
        height: tocRoot.height
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
        }

        function open() {
            if (y === openY)
                return;

            layersList.highlightFollowsCurrentItem = true;
            animateVertical.from = closedY;
            animateVertical.to = openY;
            animateVertical.start();
        }

        function close() {
            if (y === closedY)
                return;

            layersList.highlightFollowsCurrentItem = false;
            animateVertical.from = openY;
            animateVertical.to = closedY;
            animateVertical.start();
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
                    mobileMenu.close();
                    tocRoot.closed();
                    toolController.zoomTo(layersList.currentIndex);
                }
            }

            ListSeparator{}

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Remove"
                onTriggered: {
                    mobileMenu.close()
                    toolController.removeAt(layersList.currentIndex);
                }
            }

            ListSeparator {
                visible: layersList.currentIndex !== 0
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Move up"
                visible: layersList.currentIndex !== 0
                onTriggered: {
                    mobileMenu.close()
                    toolController.moveUp(layersList.currentIndex);
                }
            }

            ListSeparator {
                visible: layersList.currentIndex + 1 !== layersList.count
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Move down"
                visible: layersList.currentIndex + 1 !== layersList.count
                onTriggered: {
                    mobileMenu.close()
                    toolController.moveDown(layersList.currentIndex);
                }
            }

            ListSeparator{}

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
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
            top: tocRoot.titleBar.bottom
            margins: 15 * scaleFactor
        }
        visible: layersList.count === 0
        text: "No overlays have been added.\n\nSelect 'Add Data' to add overlays to the map."
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
    }
}

