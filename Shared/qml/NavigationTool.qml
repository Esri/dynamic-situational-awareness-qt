
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.2
import Esri.DSA 1.0

Item {
    NavigationController {
        id: navController
        objectName: "navigationController"
    }
    height: controlsColumn.height + 10 * scaleFactor
    width: controlsColumn.width + 10 * scaleFactor

    Rectangle {
        anchors.fill: parent
        color: Material.primary
        opacity: 0.5
        radius: 5 * scaleFactor
    }

    Column {
        id: controlsColumn
        anchors.centerIn: parent
        spacing: 5 * scaleFactor

        OverlayButton {
            iconUrl: "qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png"
            onClicked: {
                navController.zoomToInitialLocation();
            }
        }

        OverlayButton {
            iconUrl: "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"
            onClicked: {
                navController.zoomFactor = 2.0;
                navController.zoomIn();
            }
        }

        OverlayButton {
            iconUrl: "qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png"
            onClicked: {
                navController.zoomFactor = 0.5;
                navController.zoomOut();
            }
        }

        OverlayButton {
            iconUrl: "qrc:/Resources/icons/xhdpi/2D.png"
            onClicked: {
                navController.set2D();
            }
        }

        OverlayButton {
            iconUrl: "qrc:/Resources/icons/xhdpi/ic_menu_gpsondontfollow_dark.png"
            onClicked: {
                selected = !selected;
                followHud.enabled = selected;
                locationController.enabled = selected;
            }
        }

        OverlayButton {
            iconUrl: "qrc:/Resources/icons/xhdpi/ic_menu_refresh_dark.png"
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
