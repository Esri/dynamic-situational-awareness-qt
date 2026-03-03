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
    property real spinBoxHeight: DsaStyles.titleFontPixelSize * 1.6
    property real fontPixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor

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

            // OBSERVATIONS
            CheckBox {
                id: switchObservations
                checked: toolController.selectedFeed.showPreviousObservations
                onCheckedChanged: toolController.selectedFeed.showPreviousObservations = checked
                text: "Observations"
                font.pixelSize: fontPixelSize
            }
            GridLayout {
                visible: switchObservations.checked
                rows: 3
                columns: 2
                SpinBox {
                    id: spinObservationsSize
                    from: 1
                    to: 25
                    Layout.preferredHeight: spinBoxHeight
                    Layout.preferredWidth: drawer.width / 2.0
                    value: toolController.selectedFeed.sizeObservations
                    onValueChanged: toolController.selectedFeed.sizeObservations = value
                }
                ColorsComboBox {
                    id: colorsComboObservations
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    currentIndex: model.indexOf(toolController.selectedFeed.colorObservations)
                    onCurrentIndexChanged: toolController.selectedFeed.colorObservations = model[currentIndex]
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Size"
                    font.pixelSize: fontPixelSize
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Color"
                    font.pixelSize: fontPixelSize
                }
                Rectangle {
                    Layout.columnSpan: 2
                    color: "gray"
                    Layout.fillWidth: true
                    radius: 5
                    height: 5
                }
            }

            // TRACK LINE
            CheckBox {
                id: switchTrackLine
                checked: toolController.selectedFeed.showTrackLine
                onCheckedChanged: toolController.selectedFeed.showTrackLine = checked
                text: "Track Line"
                font.pixelSize: fontPixelSize
            }
            GridLayout {
                visible: switchTrackLine.checked
                rows: 3
                columns: 2
                SpinBox {
                    id: spinTrackLineSize
                    from: 1
                    to: 25
                    Layout.preferredHeight: spinBoxHeight
                    Layout.preferredWidth: drawer.width / 2.0
                    value: toolController.selectedFeed.sizeTrackLine
                    onValueChanged: toolController.selectedFeed.sizeTrackLine = value
                }
                ColorsComboBox {
                    id: colorsComboTrackLine
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    currentIndex: model.indexOf(toolController.selectedFeed.colorTrackLine)
                    onCurrentIndexChanged: toolController.selectedFeed.colorTrackLine = model[currentIndex]
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Size"
                    font.pixelSize: fontPixelSize
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Color"
                    font.pixelSize: fontPixelSize
                }
                Rectangle {
                    Layout.columnSpan: 2
                    color: "gray"
                    Layout.fillWidth: true
                    radius: 5
                    height: 5
                }
            }


            // TRACK LENGTH
            GridLayout {
                visible: switchObservations.checked || switchTrackLine.checked
                rows: 4
                columns: 2

                Label {
                    text: "Track Length"
                    Layout.columnSpan: 2
                    font.pixelSize: fontPixelSize
                    Layout.alignment: Qt.AlignLeft
                }
                Label {
                    text: "Amount"
                    font.pixelSize: fontPixelSize
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: drawer.width / 2.0
                }
                SpinBox {
                    id: spinObservations
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    from: 0
                    to: 9999
                    editable: true
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
                Label {
                    text: "Duration"
                    font.pixelSize: fontPixelSize
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: drawer.width / 2.0
                }
                SpinBox {
                    id: spinDuration
                    from: 0
                    to: 120
                    editable: true
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    textFromValue: function(value) {
                        if (value < 1)
                            return "---"
                        else
                            return value
                    }

                    value: toolController.selectedFeed.maximumDuration
                    onValueChanged: toolController.selectedFeed.maximumDuration = value
                }
                ComboBox {
                    id: comboDuration
                    model: ["seconds", "minutes", "hours"]
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    Layout.row: 3
                    Layout.column: 1
                    currentIndex: model.indexOf(toolController.selectedFeed.maximumDurationUnits)
                    onCurrentIndexChanged: toolController.selectedFeed.maximumDurationUnits = model[currentIndex]
                }
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
            font.pixelSize: fontPixelSize
        }

        TabButton {
            text: qsTr("Track Display")
            font.pixelSize: fontPixelSize
        }
    }
}
