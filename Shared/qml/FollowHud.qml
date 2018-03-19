
// Copyright 2017 ESRI
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
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    id: followHudRoot
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    function stopFollowing() {
        followPositionController.followMode = FollowPositionController.Disabled;
    }

    FollowPositionController {
        id: followPositionController
        onFollowModeChanged: {
            if (followPositionController.followMode === FollowPositionController.Disabled)
                followHud.state = disableLocation.name;
        }
    }

    state: disableLocation.name
    states: [
        State {
            name: disableLocation.name
            PropertyChanges {
                target: disableLocation
                selected: true
            }
        },
        State {
            name: followLocation.name
            PropertyChanges {
                target: followLocation
                selected: true
            }
        },
        State {
            name: northUpLocation.name
            PropertyChanges {
                target: northUpLocation
                selected: true
            }
        }
    ]

    visible: enabled

    onEnabledChanged: stopFollowing();

    width: backgroundRecatangle.width
    height: backgroundRecatangle.height
    property alias radius: backgroundRecatangle.radius

    Rectangle {
        id: backgroundRecatangle
        anchors {
            fill: followHudRow

        }
        color: Material.primary
        opacity: parent.opacity
    }

    Row {
        id: followHudRow

        anchors.margins: 5 * scaleFactor
        spacing: 10 * scaleFactor

        OverlayButton {
            id: disableLocation
            anchors.verticalCenter: parent.verticalCenter
            iconUrl: DsaResources.iconFollowLocationOff
            color: "transparent"
            name: "Off"
            onClicked: {
                followHudRoot.state = name;
                followPositionController.followMode = FollowPositionController.Disabled;
            }
        }

        OverlayButton {
            id: followLocation
            anchors.verticalCenter: parent.verticalCenter
            iconUrl: DsaResources.iconFollowLocation
            color: "transparent"
            name: "Follow"
            onClicked: {
                followHudRoot.state = name;
                followPositionController.followMode = FollowPositionController.TrackUp;
            }
        }

        OverlayButton {
            id: northUpLocation
            anchors.verticalCenter: parent.verticalCenter
            iconUrl: DsaResources.iconFollowLocationNorthUp
            color: "transparent"
            name: "North Up"
            onClicked: {
                followHudRoot.state = name;
                followPositionController.followMode = FollowPositionController.NorthUp;
            }
        }
    }
}
