
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

    // Create the controller
    TableOfContentsController {
        id: toolController
    }

//    property int draggedIndex: -1
//    property int droppedIndex: -1

//    Component {
//        id: dragDelegate

//        MouseArea {
//            id: dragArea

//            property bool held: false
//            property int startX: -1
//            property int startY: -1

//            anchors { left: parent.left; right: parent.right }
//            height: content.height

//            drag.target: held ? content : undefined
//            drag.axis: Drag.XAndYAxis

//            onPressAndHold: {
//                draggedIndex = index;
//                startX = mouse.x;
//                startY = mouse.y;
//                held = true;
//            }

//            onPositionChanged: {
//                if (!held)
//                    return;

//                if (drag.axis !== Drag.XAndYAxis)
//                    return;

//                var xDelta = Math.abs(startX - mouse.x);
//                var yDelta = Math.abs(startY - mouse.y);

//                if (xDelta < drag.threshold && yDelta < drag.threshold)
//                    return;

//                if (xDelta > yDelta)
//                    drag.axis = Drag.XAxis;
//                else if (yDelta > xDelta)
//                    drag.axis = Drag.YAxis;
//            }

//            onReleased: {
//                var xDelta = Math.abs(startX - mouse.x);
//                var removing = drag.axis === Drag.XAxis;
//                held = false
//                startX = -1;
//                startY= -1;
//                drag.axis = Drag.XAndYAxis;

//                if (draggedIndex === -1)
//                    return;

//                if (removing) {
//                    if (xDelta > (64 * scaleFactor))
//                        toolController.removeAt(draggedIndex);
//                }
//                else {
//                    if (droppedIndex === -1)
//                        return;

//                    if (draggedIndex === dragArea.DelegateModel.itemsIndex)
//                        return;

//                    toolController.moveFromTo(draggedIndex,
//                                              dragArea.DelegateModel.itemsIndex);
//                }
//            }

//            Rectangle {
//                id: content

//                anchors {
//                    horizontalCenter: parent.horizontalCenter
//                    verticalCenter: parent.verticalCenter
//                    margins: 8 * scaleFactor
//                }
//                width: dragArea.width
//                height: 64 * scaleFactor

//                color: dragArea.held ? Material.accent : Material.background
//                opacity: dragArea.held ? 0.5 : 1
//                Behavior on color { ColorAnimation { duration: 100 } }

//                radius: 2 * scaleFactor
//                Drag.active: dragArea.held
//                Drag.source: dragArea
//                Drag.hotSpot.x: width / 2
//                Drag.hotSpot.y: height / 2

//                states: State {
//                    when: dragArea.held

//                    ParentChange { target: content; parent: tocRoot }
//                    AnchorChanges {
//                        target: content
//                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
//                    }
//                }



//            }

//            DropArea {
//                anchors {
//                    fill: parent
//                    margins: 10 * scaleFactor
//                }

//                onEntered: {
//                    droppedIndex = dragArea.DelegateModel.itemsIndex;

//                    visualModel.items.move(drag.source.DelegateModel.itemsIndex, droppedIndex);
//                }
//            }
//        }
//    }

    // Declare the ListView, which will display the list of files
    ListView {
        id: layersList
        interactive: false
        clip: true

        anchors {
            top: tocRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }

        model: toolController.layerListModel
        width: parent.width
        spacing: 5 * scaleFactor
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
                    onClicked: menu.open()
                }

                Menu {
                    id: menu
                    MenuItem {
                        text: "Zoom To"
                        onTriggered: toolController.zoomTo(index)
                    }
                    MenuItem {
                        text: "Remove"
                        onTriggered: toolController.removeAt(index)
                    }
                    MenuItem {
                        text: "Move Up"
                        onTriggered: toolController.moveUp(index)
                    }
                    MenuItem {
                        text: "Move Down"
                        onTriggered: toolController.moveDown(index)
                    }
                }
            }

            function imageSourceForGeomType(i){
                var geomType = toolController.layerGeometryType(i);
                console.log(geomType)
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

