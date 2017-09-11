
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

    BasemapPickerController {
        id: toolController
    }

    ListView {
        id: basemapsList

        anchors{
            margins: 8 * scaleFactor
            fill: parent
        }

        spacing: 8 * scaleFactor

        clip: true
        model: toolController.tileCacheModel

        delegate: Item {

            width:  Math.min(basemapsList.width, 412)
            height: 64 * scaleFactor

            Button {
                id: button
                anchors {
                    margins: 4 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }

                height: parent.height

                text: title

                onClicked: {
                    toolController.basemapSelected(index);
                    basemapSelected();
                }

                Image {
                    id: thumbnail
                    anchors {
                        right: parent.right
                        margins: 8 * scaleFactor
                    }

                    height: parent.height
                    width: height

                    source: thumbnailUrl
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }

    Component.onCompleted: toolController.selectInitialBasemap();
}
