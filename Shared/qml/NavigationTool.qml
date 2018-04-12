
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.2
import Esri.DSA 1.0

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

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
        color: Material.primary
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
