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
import Esri.DSA 1.0

DsaPanel {
    id: messageFeedsRoot
    width: 272 * scaleFactor
    title: qsTr("Message Feeds")

    property alias controller: toolController

    // Create the controller
    MessageFeedsController {
        id: toolController
    }

    // Declare the ListView, which will display the list of message overlay feeds
    ListView {
        id: messageFeedsList

        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }

        clip: true
        model: toolController.messageFeeds
        width: parent.width
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
        }
    }
}
