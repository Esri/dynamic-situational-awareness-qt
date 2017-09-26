
// Copyright 2016 ESRI
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

Row {

    function stopFollowing() {
        followPositionController.setDisabled();
    }

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    FollowPositionController {
        id: followPositionController
    }

    spacing: 8 * scaleFactor
    visible: enabled

    onEnabledChanged: followPositionController.setDisabled();

    Button {
        id: dontFollow
        checkable: true
        checked: !followPositionController.northUp && !followPositionController.trackUp
        width: checked? 36 * scaleFactor : 32 * scaleFactor
        height: width

        background: Rectangle {
            opacity: 0
        }

        Image {
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            sourceSize.height: parent.height * 0.85
            height: sourceSize.height
            source: "qrc:/Resources/icons/xhdpi/navigation_disabled.png"
        }

        onClicked: followPositionController.setDisabled();
    }

    Button {
        id: trackUpButton
        checkable: true
        checked: followPositionController.trackUp
        visible: enabled
        width: checked? 36 * scaleFactor : 32 * scaleFactor
        height: width

        background: Rectangle {
            opacity: 0
        }

        Image {
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            sourceSize.height: parent.height * 0.85
            height: sourceSize.height
            source: "qrc:/Resources/icons/xhdpi/navigation.png"
        }

        onClicked: followPositionController.setTrackUp();
    }

    Button {
        id: northUpButton
        enabled: locationCheckBox.checked
        checkable: true
        checked: followPositionController.northUp
        visible: enabled
        width: checked? 36 * scaleFactor : 32 * scaleFactor
        height: width

        background: Rectangle {
            opacity: 0
        }

        Image {
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            sourceSize.height: parent.height * 0.85
            height: sourceSize.height
            source: "qrc:/Resources/icons/xhdpi/navigation_north.png"
        }

        onClicked: followPositionController.setNorthUp();
    }
}
