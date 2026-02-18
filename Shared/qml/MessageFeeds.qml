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
            }
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: bar.top
            }
            ComboBox {
                id: cboFeeds
                width: parent.width
                textRole: "feedName"
                model: toolController.messageFeeds
                currentIndex: toolController.selectedFeedIndex
                onCurrentIndexChanged: toolController.selectedFeedIndex = currentIndex
            }

            Text {
                id: txtObservations
                text: "Observations"
                anchors {
                    top: cboFeeds.bottom
                    left: parent.left
                    right: parent.right
                }
                color: Material.foreground
                font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
            }
            Switch {
                id: chkObservations
                anchors {
                    top: txtObservations.bottom
                    left: parent.left
                }
                checked: toolController.selectedFeedShowPreviousObservations
                onCheckedChanged: toolController.selectedFeedShowPreviousObservations = checked
            }
            Slider {
                anchors {
                    top: txtObservations.bottom
                    left: chkObservations.right
                    right: parent.right
                }
                from: 1
                to: 25
                value: toolController.selectedFeedMaximumObservations
                onValueChanged: toolController.selectedFeedMaximumObservations = value
            }

            Text {
                id: txtTracks
                text: "Tracks"
                anchors {
                    top: chkObservations.bottom
                    left: parent.left
                    right: parent.right
                }
                color: Material.foreground
                font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
            }
            Switch {
                id: chkTracks
                anchors {
                    top: txtTracks.bottom
                    left: parent.left
                }
                checked: toolController.selectedFeedShowTrackLine
                onCheckedChanged: toolController.selectedFeedShowTrackLine = checked
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
            text: qsTr("Layers")
        }

        TabButton {
            text: qsTr("Observations")
        }
    }
}
