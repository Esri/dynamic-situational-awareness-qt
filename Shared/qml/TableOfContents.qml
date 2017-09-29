
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
import QtQml.Models 2.2
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaToolBase {
    id: tocRoot

    title: "Table of contents"
    clip: true

    // Create the controller
    TableOfContentsController {
        id: toolController
    }

    property int draggedIndex: -1
    property int droppedIndex: -1

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false
            property int startX: -1
            property int startY: -1

            anchors { left: parent.left; right: parent.right }
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.XAndYAxis

            onPressAndHold: {
                draggedIndex = index;
                startX = mouse.x;
                startY = mouse.y;
                held = true;
            }

            onPositionChanged: {
                if (!held)
                    return;

                if (drag.axis !== Drag.XAndYAxis)
                    return;

                var xDelta = Math.abs(startX - mouse.x);
                var yDelta = Math.abs(startY - mouse.y);

                if (xDelta < drag.threshold && yDelta < drag.threshold)
                    return;

                if (xDelta > yDelta)
                    drag.axis = Drag.XAxis;
                else if (yDelta > xDelta)
                    drag.axis = Drag.YAxis;
            }

            onReleased: {
                var xDelta = Math.abs(startX - mouse.x);
                var removing = drag.axis === Drag.XAxis;
                held = false
                startX = -1;
                startY= -1;
                drag.axis = Drag.XAndYAxis;

                if (draggedIndex === -1)
                    return;

                if (removing) {
                    if (xDelta > (64 * scaleFactor))
                        toolController.removeAt(draggedIndex);
                }
                else {
                    if (droppedIndex === -1)
                        return;

                    if (draggedIndex === dragArea.DelegateModel.itemsIndex)
                        return;

                    toolController.moveFromTo(draggedIndex,
                                              dragArea.DelegateModel.itemsIndex);
                }
            }

            Rectangle {
                id: content

                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                    margins: 8 * scaleFactor
                }
                width: dragArea.width
                height: 64 * scaleFactor

                color: dragArea.held ? Material.accent : Material.foreground
                opacity: dragArea.held ? 0.5 : 1
                Behavior on color { ColorAnimation { duration: 100 } }

                radius: 2
                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                states: State {
                    when: dragArea.held

                    ParentChange { target: content; parent: tocRoot }
                    AnchorChanges {
                        target: content
                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                    }
                }

                CheckBox {
                    id: visibleCheckBox
                    anchors{
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    checked: layerVisible
                    onClicked: layerVisible = checked;
                }

                Text {
                    anchors{
                        left: visibleCheckBox.right
                        right: zoomToImage.left
                        verticalCenter: parent.verticalCenter
                    }

                    elide: Text.ElideRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: name && name !== "" ?
                              name :
                              toolController.getAlternateName(index)
                    verticalAlignment: Text.AlignVCenter
                    color: Material.primary
                    font.pixelSize: 14
                    font.bold: true
                }

                Image {
                    id: zoomToImage
                    fillMode: Image.PreserveAspectFit
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    sourceSize.height: 32 * scaleFactor
                    height: sourceSize.height
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_zoomtofeature_light.png"

                    MouseArea {
                        anchors.fill: parent

                        onClicked: toolController.zoomTo(index);
                    }
                }

            }

            DropArea {
                anchors { fill: parent; margins: 10 }

                onEntered: {
                    droppedIndex = dragArea.DelegateModel.itemsIndex;

                    visualModel.items.move(drag.source.DelegateModel.itemsIndex, droppedIndex);
                }
            }
        }
    }

    DelegateModel {
        id: visualModel

        model: toolController.layerListModel
        delegate: dragDelegate
    }

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

        model: visualModel
        width: parent.width

    }



    //    SwipeDelegate {
    //    id: delegate
    //    clip: false
    //    width: parent.width
    //    height: 32 * scaleFactor

    //    contentItem: Row {
    //        height: parent.height
    //        anchors.verticalCenter: parent.verticalCenter

    //        CheckBox {
    //            anchors.verticalCenter: parent.verticalCenter
    //            checked: layerVisible
    //            onClicked: layerVisible = checked;
    //        }

    //        Text {
    //            anchors.verticalCenter: parent.verticalCenter
    //            width: 128 * scaleFactor
    //            elide: Text.ElideRight
    //            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    //            text: name && name !== "" ?
    //                      name :
    //                      toolController.getAlternateName(index)
    //            verticalAlignment: Text.AlignVCenter
    //            color: Material.primary
    //        }

    //        Image {
    //            fillMode: Image.PreserveAspectFit
    //            anchors.verticalCenter: parent.verticalCenter
    //            sourceSize.height: 32 * scaleFactor
    //            height: sourceSize.height
    //            source: "qrc:/Resources/icons/xhdpi/ic_menu_zoomtofeature_light.png"

    //            MouseArea {
    //                anchors.fill: parent

    //                onClicked: toolController.zoomTo(index);
    //            }
    //        }
    //    }

    //    background: Rectangle {
    //        color: Material.foreground
    //    }

    //    swipe.right: Rectangle {
    //        anchors.fill: parent
    //        color: Material.background
    //    }

    //    swipe.left: Rectangle {
    //        anchors.fill: parent
    //        color: Material.background
    //    }

    //    swipe.onCompleted: toolController.removeAt(index);

    ////            MouseArea {
    ////                id: dragArea

    ////                property bool held: false

    ////                anchors { left: delegate.left; right: delegate.right }
    ////                height: delegate.height

    ////                drag.target: held ? content : undefined
    ////                drag.axis: Drag.YAxis

    ////                onPressAndHold: {
    ////                    draggedIndex = index;
    ////                    held = true;
    ////                }
    ////                onReleased: {
    //////                    draggedIndex = -1;
    ////                    held = false
    ////                }

    ////                Rectangle {
    ////                    id: content
    ////                    color: Material.accent
    ////                    opacity: 0.5
    ////                    height: dragArea.held ? delegate.height : 0
    ////                    width: dragArea.held ? delegate.width : 0
    ////                    Drag.active: dragArea.held
    ////                    Drag.source: dragArea
    ////                    Drag.hotSpot.x: width / 2
    ////                    Drag.hotSpot.y: height / 2
    ////                }
    ////                DropArea {
    ////                    anchors { fill: parent; margins: 10 }

    ////                    onEntered: {
    ////                        console.log(draggedIndex, index);

    ////                        console.log(
    ////                                    drag.source.DelegateModel.itemsIndex,
    ////                                    dragArea.DelegateModel.itemsIndex)
    ////                    }


    ////                }
    ////            }

    //}

}

