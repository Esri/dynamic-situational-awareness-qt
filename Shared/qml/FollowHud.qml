
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

Row {
    id: followHudRoot
    function stopFollowing() {
        followPositionController.followMode = FollowPositionController.Disabled;
    }

    FollowPositionController {
        id: followPositionController
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

    spacing: 10 * scaleFactor
    visible: enabled

    onEnabledChanged: stopFollowing();

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
