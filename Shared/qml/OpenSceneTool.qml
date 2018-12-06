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
    title: packageFrame.currentIndex === 0 ? "Choose Package" : "Open Scene"

    signal sceneSelected();

    OpenMobileScenePackageController {
        id: toolController
    }

    Button {
        id: backToPackagesButton

        anchors {
            top: titleBar.bottom
            left: packageFrame.left
            margins: 8 * scaleFactor
        }

        visible: packageFrame.currentIndex === 1

        text: "< Packages"

        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        font.family: DsaStyles.fontFamily

        onClicked: packageFrame.currentIndex = 0;
    }

    SwipeView {
        id: packageFrame
        clip: true
        anchors{
            top: backToPackagesButton.bottom
//            left: parent.left
//            right: parent.right
            horizontalCenter: parent.horizontalCenter
            bottom: footerBar.top
            margins: 8 * scaleFactor
        }
        width: parent.width
        currentIndex: 0

        GridView {
            id: packagesList

            visible: packageFrame.currentIndex === 0

            clip: true
            model: toolController.packages

            cellWidth: 128 * scaleFactor
            cellHeight: 128 * scaleFactor
            width: 2 * cellWidth

            delegate: Rectangle {
                id: packageCard

                property variant documentsModel: documents

                width: packagesList.cellWidth - 10 * scaleFactor
                height: packagesList.cellHeight - 10 * scaleFactor

                border.color: index === packagesList.currentIndex ? Material.accent : Material.background
                border.width: index === packagesList.currentIndex ? 2 * scaleFactor : 1 * scaleFactor
                color: Material.background
                radius: 2 * scaleFactor

                Image {
                    source: imageReady ? "image://packages/" + packageName : ""
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
                    text: packageName
                    anchors.centerIn: parent
                    width: parent.width - (16 * scaleFactor)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    wrapMode: Text.WrapAnywhere
                    font{
                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        bold: true
                    }
                    color: Material.foreground
                }

                Label {
                    id: needsUnpackLabel
                    visible: requiresUnpack
                    text: "Needs Unpack"
                    anchors {
                        top: packageTitle.bottom
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    wrapMode: Text.WrapAnywhere
                    font{
                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        bold: true
                    }
                    color: Material.accent
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        // If the package has an unpacked version, use that
                        toolController.selectPackageName( unpackedName.length > 0 ? unpackedName :
                                                                                    packageName);
                        unpackButton.visible = requiresUnpack
                        packageFrame.currentIndex = 1;
                    }
                    onHoveredChanged: {
                        if (containsMouse) {
                            packagesList.currentIndex = index
                        }
                    }
                }
            }
        }

        ListView {
            id: documentList

            model: packagesList.currentItem.documentsModel

            clip: true
            spacing: 64 * scaleFactor
            visible: packageFrame.currentIndex === 1

            Button {
                id: unpackButton
                anchors.centerIn: parent

                contentItem: Text {
                    anchors.fill: parent
                    text: "Unpack"
                    font{
                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        family: DsaStyles.fontFamily
                        bold: true
                    }
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: Material.accent
                }

                onClicked: {
                    toolController.unpack();
                    unpackButton.visible = false;
                }
            }

            Label
            {
                id: unpackText
                visible: unpackButton.visible
                anchors {
                    top: unpackButton.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    margins: 8 * scaleFactor
                }

                text: "This package contains archived data that must be unpacked to a directory on your device"
                wrapMode: Text.WordWrap

                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.family: DsaStyles.fontFamily

            }

            delegate: Button {
                text: modelData
                width: parent.width
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.family: DsaStyles.fontFamily
                onClicked: {
                    toolController.selectDocument(index);
                    sceneSelected();
                }
            }
        }
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
            text: toolController.currentPackageName
            wrapMode: Text.WrapAnywhere
            elide: Text.ElideRight
            font.pixelSize: 12 * scaleFactor
        }
    }
}
