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

    property string packageTitleString
    property string packageDescriptionString

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

        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }

        onClicked: packageFrame.currentIndex = 0
    }

    Button {
        id: resetButton
        anchors {
            top: titleBar.bottom
            left: packageFrame.left
            right: packageFrame.right
            margins: 8 * scaleFactor
        }

        visible: packageFrame.currentIndex === 0

        text: "Reset to Default Scene"

        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }

        onClicked: {
            toolController.createDefaultScene();
            sceneSelected();
        }
    }

    SwipeView {
        id: packageFrame
        anchors {
            top: backToPackagesButton.bottom
            horizontalCenter: parent.horizontalCenter
            bottom: footerBar.top
            margins: 8 * scaleFactor
        }

        clip: true
        width: parent.width
        currentIndex: 0

        GridView {
            id: packagesList

            visible: packageFrame.currentIndex === 0

            clip: true
            model: toolController.packages

            anchors.horizontalCenter: parent.horizontalCenter
            cellWidth: packagesList.width * 0.5
            cellHeight: cellWidth

            delegate: Item {
                width: packagesList.cellWidth
                height: width

                property variant sceneNamesModel: sceneNames
                property bool sceneImages: sceneImagesReady

                Rectangle {
                    id: packageCard

                    anchors {
                        fill: parent
                        margins: 8 * scaleFactor
                    }

                    border.color: index === packagesList.currentIndex ? Material.accent : Material.background
                    border.width: index === packagesList.currentIndex ? 2 * scaleFactor : 1 * scaleFactor
                    color: Material.background
                    radius: 2 * scaleFactor

                    Image {
                        anchors {
                            fill: parent
                            margins: 4 * scaleFactor
                        }

                        source: imageReady ? "image://packages/" + packageName : ""
                        fillMode: Image.PreserveAspectCrop
                    }

                    Rectangle {
                        anchors.centerIn: packageTitleLabel
                        width: packageTitleLabel.width + (8 * scaleFactor)
                        height: packageTitleLabel.height + (8 * scaleFactor)
                        color: Material.primary
                        opacity: 0.5
                        radius: 2 * scaleFactor
                    }

                    Label {
                        id: packageTitleLabel
                        anchors.centerIn: parent
                        text: packageName
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
                        anchors {
                            top: packageTitleLabel.bottom
                            bottom: parent.bottom
                            left: parent.left
                            right: parent.right
                        }

                        visible: requiresUnpack
                        text: "Needs Unpack"

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        wrapMode: Text.WrapAnywhere
                        font {
                            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                            bold: true
                        }
                        color: Material.accent
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            packageTitleString = packageTitle;
                            packageDescriptionString = packageDescription;
                            packageFrame.currentIndex = 1;
                            // If the package has an unpacked version, use that
                            toolController.selectPackageName( unpackedName.length > 0 ? unpackedName :
                                                                                        packageName);
                            unpackButton.visible = requiresUnpack
                        }

                        onHoveredChanged: {
                            if (containsMouse) {
                                packagesList.currentIndex = index
                            }
                        }
                    }
                }
            }
        }

        ListView {
            id: sceneNamesList

            model: packagesList.currentItem ? packagesList.currentItem.sceneNamesModel : null

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

            Label {
                id: unpackText
                anchors {
                    top: unpackButton.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    margins: 8 * scaleFactor
                }
                visible: unpackButton.visible

                text: "This package contains archived data that must be unpacked to a directory on your device"
                wrapMode: Text.WordWrap

                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.family: DsaStyles.fontFamily
            }

            delegate: Rectangle {
                width: parent.width
                height: width * 0.4

                border.color: index === sceneNamesList.currentIndex ? Material.accent : Material.background
                border.width: index === sceneNamesList.currentIndex ? 2 * scaleFactor : 1 * scaleFactor
                color: Material.background
                radius: 2 * scaleFactor

                Image {
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                        right: parent.horizontalCenter
                        margins: 4 * scaleFactor
                    }

                    source: packagesList.currentItem.sceneImages ? "image://packages/" + toolController.currentPackageName + "_" + modelData
                                                                 : ""
                    fillMode: Image.PreserveAspectCrop
                }

                Label {
                    text: modelData
                    anchors {
                        left: parent.horizontalCenter
                        top: parent.top
                        bottom: parent.bottom
                        right: parent.right
                        margins: 4 * scaleFactor
                    }

                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    font.family: DsaStyles.fontFamily
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        toolController.selectScene(index);
                        sceneSelected();
                    }

                    onHoveredChanged: {
                        if (containsMouse) {
                            sceneNamesList.currentIndex = index
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: footerBar
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        visible: packageFrame.currentIndex == 1
        color: Material.primary

        height: 64 * scaleFactor

        Column {
            anchors.fill: parent
            spacing: 4 * scaleFactor

            Label {
                text: "Current Package: " + (packageTitleString.length > 0 ? packageTitleString : toolController.currentPackageName)
                wrapMode: Text.WrapAnywhere
                elide: Text.ElideRight
                font.pixelSize: 12 * scaleFactor
            }

            Label {
                visible: text.length > 13
                text: "Description: " + packageDescriptionString
                wrapMode: Text.WrapAnywhere
                elide: Text.ElideRight
                font.pixelSize: 12 * scaleFactor
            }
        }
    }
}

