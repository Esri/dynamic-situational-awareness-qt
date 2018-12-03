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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    width: parent.width
    title: qsTr("Open Package")

    signal packageSelected();

    OpenPackageController {
        id: toolController
    }

    Component {
        id: packageDelegate

        Rectangle {
            id: packageCard
            width: packagesList.cellWidth - 10 * scaleFactor
            height: packagesList.cellHeight - 10 * scaleFactor

            border.color: index === packagesList.currentIndex ? Material.accent : Material.background
            border.width: index === packagesList.currentIndex ? 2 * scaleFactor : 1 * scaleFactor
            color: Material.background
            radius: 2 * scaleFactor

            Image {
                source: ""// thumbnailUrl;
                fillMode: Image.PreserveAspectCrop
                anchors{
                    fill: parent
                    margins: 4 * scaleFactor
                }
            }

            Rectangle {
                anchors.centerIn: packageTitle
                width: packageTitle.width + (8 * scaleFactor)
                height: packageTitle.height + (8 * scaleFactor)
                color: Material.primary
                opacity: 0.5
                radius: 2 * scaleFactor
            }

            Label {
                id: packageTitle
                text: modelData
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
                hoverEnabled: true
                onClicked: {
                    toolController.selectPackageName(modelData);
                    packageSelected();
                }
                onHoveredChanged: {
                    if (containsMouse) {
                        packagesList.currentIndex = index
                    }
                }

            }
        }
    }

    GridView {
        id: packagesList

        property string currentPath: ""

        anchors{
            top: titleBar.bottom
            horizontalCenter: parent.horizontalCenter
            bottom: footerBar.top
            margins: 8 * scaleFactor
        }

        clip: true
        model: toolController.packageNames

        cellWidth: 128 * scaleFactor
        cellHeight: 128 * scaleFactor
        width: 2 * cellWidth

        delegate: packageDelegate
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
            text: packagesList.currentPath
            wrapMode: Text.WrapAnywhere
            elide: Text.ElideRight
            font.pixelSize: 12 * scaleFactor

        }
    }
}
