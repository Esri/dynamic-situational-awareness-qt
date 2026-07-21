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
import QtQuick.Layouts
import Esri.ArcGISRuntime.OpenSourceApps.DSA

DsaPanel {
    id: messageFeedsRoot
    width: 272 * scaleFactor
    title: qsTr("Message Feeds")

    property alias controller: toolController
    property bool isMobile
    property real spinBoxHeight: DsaStyles.titleFontPixelSize * 1.6
    property real fontPixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
    property real detailLabelFontPixelSize: 10 * scaleFactor
    property real trackControlSpacing: 4 * scaleFactor
    property real trackSectionSpacing: 18 * scaleFactor
    property real trackDividerSpacing: 20 * scaleFactor

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
            clip: true
            model: toolController.messageFeeds
            delegate:  ListItemDelegate {
                id: control
                width: parent.width
                height: 40 * scaleFactor
                mainText: feedName
                itemChecked: feedVisible
                highlighted: toolController.selectedFeedIndex === index
                clickTogglesCheck: false
                rowTapExcludesCheckBox: true
                onRowTapped: {
                    toolController.selectedFeedIndex = index;
                }
                onItemCheckedChanged: {
                    if (feedVisible === itemChecked)
                        return;

                    feedVisible = itemChecked
                }
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
                            toolController.selectedFeedIndex = index;
                            bar.currentIndex = 1;
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 0

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
                Layout.topMargin: trackSectionSpacing
                checked: toolController.selectedFeed.showPreviousObservations
                onCheckedChanged: toolController.selectedFeed.showPreviousObservations = checked
                text: "Observations"
                font.pixelSize: fontPixelSize
            }
            GridLayout {
                visible: switchObservations.checked
                Layout.fillWidth: true
                Layout.topMargin: trackControlSpacing
                rows: 2
                columns: 2
                rowSpacing: trackControlSpacing
                columnSpacing: 8 * scaleFactor
                SpinBox {
                    id: spinObservationsSize
                    from: 1
                    to: 25
                    Layout.preferredHeight: spinBoxHeight
                    Layout.preferredWidth: drawer.width / 2.0
                    value: toolController.selectedFeed.sizeObservations
                    onValueChanged: {
                        if (!toolController.selectedFeed)
                            return;

                        toolController.selectedFeed.sizeObservations = value
                    }
                }
                ColorsComboBox {
                    id: colorsComboObservations
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    currentIndex: model.indexOf(toolController.selectedFeed.colorObservations)
                    onCurrentIndexChanged: {
                        if (!toolController.selectedFeed)
                            return;

                        toolController.selectedFeed.colorObservations = model[currentIndex]
                    }
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Size"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Color"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
            }

            // TRACK LINE
            CheckBox {
                id: switchTrackLine
                Layout.topMargin: switchObservations.checked ? trackSectionSpacing : 0
                checked: toolController.selectedFeed.showTrackLine
                onCheckedChanged: toolController.selectedFeed.showTrackLine = checked
                text: "Track Line"
                font.pixelSize: fontPixelSize
            }
            GridLayout {
                visible: switchTrackLine.checked
                Layout.fillWidth: true
                Layout.topMargin: trackControlSpacing
                rows: 2
                columns: 2
                rowSpacing: trackControlSpacing
                columnSpacing: 8 * scaleFactor
                SpinBox {
                    id: spinTrackLineSize
                    from: 1
                    to: 25
                    Layout.preferredHeight: spinBoxHeight
                    Layout.preferredWidth: drawer.width / 2.0
                    value: toolController.selectedFeed.sizeTrackLine
                    onValueChanged: {
                        if (!toolController.selectedFeed)
                            return;

                        toolController.selectedFeed.sizeTrackLine = value
                    }
                }
                ColorsComboBox {
                    id: colorsComboTrackLine
                    Layout.preferredHeight: spinBoxHeight
                    Layout.fillWidth: true
                    currentIndex: model.indexOf(toolController.selectedFeed.colorTrackLine)
                    onCurrentIndexChanged: {
                        if (!toolController.selectedFeed)
                            return;

                        toolController.selectedFeed.colorTrackLine = model[currentIndex]
                    }
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Size"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: "Color"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
            }

            // TRACK LENGTH
            ColumnLayout {
                visible: switchObservations.checked || switchTrackLine.checked
                Layout.fillWidth: true
                Layout.topMargin: trackDividerSpacing
                spacing: trackSectionSpacing

                Rectangle {
                    color: "gray"
                    Layout.fillWidth: true
                    radius: 5
                    height: 5
                }

                Label {
                    text: "Track Length"
                    font.pixelSize: fontPixelSize
                    Layout.alignment: Qt.AlignHCenter
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: trackControlSpacing

                    SpinBox {
                        id: spinObservations
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: spinBoxHeight
                        Layout.preferredWidth: drawer.width / 2.0
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
                        text: "Amount"
                        font.pixelSize: detailLabelFontPixelSize
                        font.italic: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }

        ColumnLayout {

            ComboBox {
                id: comboFeedsFind
                Layout.fillWidth: true
                textRole: "feedName"
                model: toolController.messageFeeds
                currentIndex: toolController.selectedFeedIndex
                onCurrentIndexChanged: toolController.selectedFeedIndex = currentIndex
            }

            TextField {
                id: textFindEntity
                Layout.fillWidth: true
                onTextChanged: toolController.findEntities(text);

                Button {
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                        margins: 2 * scaleFactor
                    }
                    visible: textFindEntity.text !== ""

                    width: height

                    background: Rectangle {
                        anchors.fill: parent
                        color: Material.primary
                    }

                    Image {
                        anchors.fill: parent
                        source: DsaResources.iconClose
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        textFindEntity.text = "";
                        toolController.clearSearchResults();
                    }
                }
            }

            ListView {
                id: listEntityResults
                model: toolController.entityIdResults
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                delegate: ItemDelegate {
                    text: model.display
                    width: listEntityResults.width
                    MouseArea {
                        anchors {
                            fill: parent
                        }
                        onClicked: {
                            toolController.selectEntity(index);
                            if (isMobile) {
                                mapToolRow.reset();
                            }
                        }
                    }
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
            display: AbstractButton.IconOnly
            icon.source: DsaResources.iconListView
            icon.color: Material.foreground
            font.pixelSize: fontPixelSize
        }

        TabButton {
            display: AbstractButton.IconOnly
            icon.source: DsaResources.iconColorPalette
            icon.color: Material.foreground
            font.pixelSize: fontPixelSize
        }

        TabButton {
            display: AbstractButton.IconOnly
            icon.source: DsaResources.iconZoomTo
            icon.color: Material.foreground
            font.pixelSize: fontPixelSize
        }
    }
}
