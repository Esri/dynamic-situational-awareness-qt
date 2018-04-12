// Copyright 2018 ESRI
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
