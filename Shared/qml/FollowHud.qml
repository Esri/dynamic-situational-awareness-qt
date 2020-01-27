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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1

Item {
    id: followHudRoot
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

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
