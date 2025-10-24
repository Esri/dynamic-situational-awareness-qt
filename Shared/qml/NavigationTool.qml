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
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Controls.Material
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    NavigationController {
        id: navController
        objectName: "navigationController"
    }
    height: controlsColumn.height + 10 * scaleFactor
    width: controlsColumn.width + 10 * scaleFactor
    property alias radius: backgroundRectangle.radius

    function startFollowing() {
        followingButton.selected = true;
        followHud.enabled = true;
        locationController.locationVisible = true;
    }

    Rectangle {
        id: backgroundRectangle
        anchors.fill: parent
        color: Material.background
        opacity: parent.opacity
    }

    Column {
        id: controlsColumn
        anchors.centerIn: parent
        spacing: 5 * scaleFactor

        OverlayButton {
            iconName: CalciteIcons.Calcite_home
            onClicked: {
                followHud.stopFollowing();
                navController.zoomToInitialLocation();
            }
        }

        OverlayButton {
            iconName: CalciteIcons.Calcite_plus
            onClicked: {
                navController.zoomFactor = 2.0;
                navController.zoomIn();
            }
        }

        OverlayButton {
            iconName: CalciteIcons.Calcite_minus
            onClicked: {
                navController.zoomFactor = 0.5;
                navController.zoomOut();
            }
        }

        OverlayButton {
            iconName: CalciteIcons.Calcite_2d
            onClicked: {
                navController.set2D();
            }
        }

        OverlayButton {
            id: followingButton
            iconName: selected ? CalciteIcons.Calcite_gps_on : CalciteIcons.Calcite_gps_off
            onClicked: {
                selected = !selected;
                followHud.enabled = selected;
                locationController.locationVisible = selected;
            }
        }

        OverlayButton {
            iconName: CalciteIcons.Calcite_rotate
            onClicked: {
                selected = !selected;
                if (selected)
                    navController.setRotation();
                else
                    navController.pan();
            }
        }

        OverlayButton {
            id: overlayButtonGrid
            iconName: CalciteIcons.Calcite_coordinate_system
            Component.onCompleted: selected = gridController.gridVisible
            onClicked: {
                selected = !selected;
                gridController.gridVisible = selected;
            }
        }
    }
}
