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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

DsaPanel {
    signal basemapSelected();

    width: Math.min(parent.width, ((basemapsList.cellWidth * 2) + (16 * scaleFactor)))
    title: qsTr("Basemaps")

    BasemapPickerController {
        id: toolController
    }

    Component {
        id: tileCacheDelegate

        Rectangle {
            id: basemapCard
            width: basemapsList.cellWidth - 10 * scaleFactor
            height: basemapsList.cellHeight - 10 * scaleFactor

            border.color: index === toolController.selectedBasemapIndex ? Material.accent : Material.background
            border.width: index === toolController.selectedBasemapIndex ? 2 * scaleFactor : 1 * scaleFactor
            color: Material.background
            radius: 2 * scaleFactor

            Image {
                source: thumbnailUrl;
                fillMode: Image.PreserveAspectCrop
                anchors{
                    fill: parent
                    margins: 4 * scaleFactor
                }
            }

            Rectangle {
                anchors.centerIn: tileCacheTitle
                width: tileCacheTitle.width + (8 * scaleFactor)
                height: tileCacheTitle.height + (8 * scaleFactor)
                color: Material.primary
                opacity: 0.5
                radius: 2 * scaleFactor
            }

            Label {
                id: tileCacheTitle
                text: title;
                anchors.centerIn: parent
                width: parent.width - (16 * scaleFactor)
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                wrapMode: Text.WrapAnywhere
                font.bold: true
                color: Material.foreground
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (index !== toolController.selectedBasemapIndex) {
                        toolController.basemapSelected(index);
                        basemapSelected();
                    }
                }
            }
        }
    }

    GridView {
        id: basemapsList

        anchors{
            top: titleBar.bottom
            horizontalCenter: parent.horizontalCenter
            bottom: footerBar.top
            margins: 8 * scaleFactor
        }

        clip: true
        model: toolController.tileCacheModel

        cellWidth: 128 * scaleFactor
        cellHeight: 128 * scaleFactor
        width: 2 * cellWidth
        currentIndex: toolController.selectedBasemapIndex

        delegate: tileCacheDelegate
    }


    Rectangle {
        id: footerBar
        anchors{
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        color: Material.primary

        height: 32 * scaleFactor

        Label {
            id: pathText
            anchors.fill: parent
            text: toolController.selectedBasemapPath
            wrapMode: Text.WrapAnywhere
            elide: Text.ElideRight
            font.pixelSize: 12 * scaleFactor

        }
    }

    Component.onCompleted: toolController.selectInitialBasemap();
}
