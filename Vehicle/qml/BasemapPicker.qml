
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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.Vehicle 1.0

Item {
    signal basemapSelected();

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    anchors {
        margins: 32
        left: parent.left
        right: parent.right
    }

    BasemapPickerController {
        id: toolController
    }

    Component {
        id: tileCacheDelegate
        Item {
            width: basemapsList.cellWidth;
            height: basemapsList.cellHeight

            Image {
                source: thumbnailUrl;
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
            }
            Label {
                id: button
                text: title;
                anchors.centerIn: parent

                font.bold: true
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    toolController.basemapSelected(index);
                    basemapSelected();
                }
            }
        }
    }

    GridView {
        id: basemapsList

        anchors{
            fill: parent
            margins: 32
        }

        clip: true
        model: toolController.tileCacheModel

        cellWidth: 256; cellHeight: 256

        delegate: tileCacheDelegate
    }

    Component.onCompleted: toolController.selectInitialBasemap();
}
