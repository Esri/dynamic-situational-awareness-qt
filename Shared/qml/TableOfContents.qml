/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

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

    onVisibleChanged: {
        if (visible) {
            toolController.updateLayerListModel();
            layersList.currentIndex = -1;
        }
    }

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
            radius: 5 * scaleFactor
            color: Material.accent
            opacity: 0.5
        }
        highlightFollowsCurrentItem: isMobile
        highlightMoveVelocity: 10000
        delegate: ListItemDelegate {
            width: parent.width
            height: 40 * scaleFactor
            itemChecked: layerVisible
            imageUrl: imageSourceForGeomType(index)
            imageVisible: true
            imageFrameVisible: false
            menuIconVisible: true
            mainText: name && name !== "" ?
                          name :
                          toolController.alternateName(index)
            onItemCheckedChanged: layerVisible = itemChecked

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
                Menu {
                    id: vehicleMenu
                    width: 125 * scaleFactor

                    Column {
                        width: parent.width
                        spacing: 10 * scaleFactor

                        ListLabel {
                            text: "Zoom to"
                            separatorVisible: visible
                            onTriggered: {
                                vehicleMenu.close();
                                tocRoot.closed();
                                toolController.zoomTo(layersList.currentIndex);
                            }
                        }

                        ListLabel {
                            text: "Remove"
                            separatorVisible: moveUpLabel.visible || moveDownLabel.visible
                            onTriggered: {
                                vehicleMenu.close()
                                toolController.removeAt(layersList.currentIndex);
                            }
                        }

                        ListLabel {
                            id: moveUpLabel
                            text: "Move up"
                            visible: layersList.currentIndex !== 0
                            separatorVisible: layersList.currentIndex !== layersList.count -1
                            onTriggered: {
                                vehicleMenu.close()
                                toolController.moveUp(layersList.currentIndex);
                            }
                        }

                        ListLabel {
                            id: moveDownLabel
                            text: "Move down"
                            visible: layersList.currentIndex + 1 !== layersList.count
                            separatorVisible: false
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
                case TableOfContentsController.FreehandMarkup:
                    return DsaResources.iconSketch;
                case TableOfContentsController.Kml:
                    return DsaResources.iconKml;
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
            border {
                color: Material.primary
                width: 1 * scaleFactor
            }
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
            layersList.currentIndex = -1;
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
                    var i = layersList.currentIndex;
                    mobileMenu.close();
                    tocRoot.closed();
                    toolController.zoomTo(i);
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Remove"
                onTriggered: {
                    toolController.removeAt(layersList.currentIndex);
                    mobileMenu.close()
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Move up"
                visible: layersList.currentIndex !== 0
                onTriggered: {
                    toolController.moveUp(layersList.currentIndex);
                    mobileMenu.close()
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Move down"
                visible: layersList.currentIndex + 1 !== layersList.count
                onTriggered: {
                    toolController.moveDown(layersList.currentIndex);
                    mobileMenu.close()
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Cancel"
                separatorVisible: false
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

