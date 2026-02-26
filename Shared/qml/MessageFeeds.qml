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
import Esri.ArcGISRuntime.OpenSourceApps.DSA
import QtQuick.Layouts

DsaPanel {
    id: messageFeedsRoot
    width: 272 * scaleFactor
    title: qsTr("Message Feeds")

    property alias controller: toolController
    property bool isMobile

    // Create the controller
    MessageFeedsController {
        id: toolController
    }

    onVisibleChanged: {
        // always switch back to the 'Feeds' tab if the panel is hidden
        bar.currentIndex = 0;
    }

    StackLayout {
        currentIndex: bar.currentIndex

        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: bar.top
            margins: 8 * scaleFactor
        }

        ListView {
            id: messageFeedsList

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: bar.top
            }

            clip: true
            model: toolController.messageFeeds
            delegate:  ListItemDelegate {
                id: control
                width: parent.width
                height: 40 * scaleFactor
                mainText: feedName
                itemChecked: feedVisible
                onItemCheckedChanged: feedVisible = itemChecked
                imageUrl: thumbnailUrl
                imageVisible: true
                imageFrameVisible: false
                menuIconVisible: true

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
                            // select the feed that was clicked on in the combo box
                            // in the 'Tracks' panel
                            comboFeeds.currentIndex = index;
                            bar.currentIndex = 1;
                        }
                    }
                }
            }
        }
        ColumnLayout {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: bar.top
            }

            ComboBox {
                id: comboFeeds
                Layout.fillWidth: true
                textRole: "feedName"
                model: toolController.messageFeeds
                currentIndex: toolController.selectedFeedIndex
                onCurrentIndexChanged: toolController.selectedFeedIndex = currentIndex
            }

            Switch {
                id: switchObservations
                checked: toolController.selectedFeed.showPreviousObservations
                onCheckedChanged: toolController.selectedFeed.showPreviousObservations = checked
                text: "Observations"
                font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
            }

            RowLayout {
                visible: switchObservations.checked
                ColumnLayout {
                    SpinBox {
                        id: spinObservationsSize
                        from: 1
                        to: 25
                    }
                    Label {
                        Layout.alignment: Qt.AlignCenter
                        text: "Size"
                        font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
                    }
                }

                ColumnLayout {
                    ColorsComboBox {
                        id: colorsComboObservations
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.maximumHeight: spinObservationsSize.height
                        currentIndex: colorsComboObservations.model.indexOf(toolController.selectedFeed.colorObservations)
                        onCurrentIndexChanged: toolController.selectedFeed.colorObservations = colorsComboObservations.model[currentIndex]
                    }
                    Label {
                        Layout.alignment: Qt.AlignCenter
                        text: "Color"
                        font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
                    }
                }
            }
            Rectangle {
                visible: switchObservations.checked
                color: "gray"
                Layout.fillWidth: true
                radius: 5
                height: 5
            }

            Switch {
                id: switchTrackLine
                checked: toolController.selectedFeed.showTrackLine
                onCheckedChanged: toolController.selectedFeed.showTrackLine = checked
                text: "Track Line"
                font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
            }
            RowLayout {
                visible: switchTrackLine.checked
                ColumnLayout {
                    SpinBox {
                        id: spinTrackLineSize
                        from: 1
                        to: 25
                    }
                    Label {
                        Layout.alignment: Qt.AlignCenter
                        text: "Size"
                        font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
                    }
                }

                ColumnLayout {
                    ColorsComboBox {
                        id: colorsComboTrackLine
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.maximumHeight: spinTrackLineSize.height
                        currentIndex: colorsComboTrackLine.model.indexOf(toolController.selectedFeed.colorTrackLine)
                        onCurrentIndexChanged: toolController.selectedFeed.colorTrackLine = colorsComboTrackLine.model[currentIndex]
                    }
                    Label {
                        Layout.alignment: Qt.AlignCenter
                        text: "Color"
                        font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
                    }
                }
            }
            Rectangle {
                visible: switchTrackLine.checked
                color: "gray"
                Layout.fillWidth: true
                radius: 5
                height: 5
            }

            SpinBox {
                id: spinObservations
                Layout.fillWidth: true
                from: 0
                to: 9999
                editable: true
                visible: switchObservations.checked || switchTrackLine.checked
                live: true
                textFromValue: function(value) {
                    if (value < 1)
                        return "All"
                    else
                        return value
                }

                value: toolController.selectedFeed.maximumObservations
                onValueChanged: toolController.selectedFeed.maximumObservations = value
            }
        }
    }

    TabBar {
        id: bar
        anchors {
            bottom: parent.bottom
        }
        width: parent.width

        TabButton {
            text: qsTr("Feeds")
            font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
        }

        TabButton {
            text: qsTr("Track Display")
            font.pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
        }
    }
}
