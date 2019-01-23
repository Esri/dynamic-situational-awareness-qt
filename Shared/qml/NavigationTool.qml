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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.2
import Esri.DSA 1.0

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    NavigationController {
        id: navController
        objectName: "navigationController"
    }
    height: controlsColumn.height + 10 * scaleFactor
    width: controlsColumn.width + 10 * scaleFactor
    property alias radius: backgroundRecatangle.radius

    function startFollowing() {
        followingButton.selected = true;
        followHud.enabled = true;
        locationController.locationVisible = true;
    }

    Rectangle {
        id: backgroundRecatangle
        anchors.fill: parent
        color: Material.background
        opacity: parent.opacity
    }

    Column {
        id: controlsColumn
        anchors.centerIn: parent
        spacing: 5 * scaleFactor

        OverlayButton {
            iconUrl: DsaResources.iconHome
            onClicked: {
                navController.zoomToInitialLocation();
            }
        }

        OverlayButton {
            iconUrl: DsaResources.iconAdd
            onClicked: {
                navController.zoomFactor = 2.0;
                navController.zoomIn();
            }
        }

        OverlayButton {
            iconUrl: DsaResources.iconRemove
            onClicked: {
                navController.zoomFactor = 0.5;
                navController.zoomOut();
            }
        }

        OverlayButton {
            iconUrl: DsaResources.icon2d
            onClicked: {
                navController.set2D();
            }
        }

        OverlayButton {
            id: followingButton
            iconUrl: selected ? DsaResources.iconGps : DsaResources.iconGpsOff
            onClicked: {
                selected = !selected;
                followHud.enabled = selected;
                locationController.locationVisible = selected;
            }
        }

        OverlayButton {
            iconUrl: DsaResources.iconRotate
            onClicked: {
                selected = !selected;
                if (selected)
                    navController.setRotation();
                else
                    navController.pan();
            }
        }
    }
}
