
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
    }
        // The following buttons have been commented out until we have public API to
        // to use an existing distance from the camera.

        /*
        Button {
            id: pan
            width: 32 * scaleFactor
            height: 32 * scaleFactor
            checkable: true
            checked: false
            ButtonGroup.group: panButtonGroup3D
            text: "Pan"

            background: Rectangle {
                implicitHeight: 40 * scaleFactor
                implicitWidth: 40 * scaleFactor
                anchors.fill: pan
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                width: 26 * scaleFactor
                height: width
//                source: navController.vertical ? "qrc:/Resources/icons/xhdpi/3D.png" : "qrc:/Resources/icons/xhdpi/2D.png"
            }

            onClicked: {
                navController.pan();
            }
        }

        Button {
            id: rotate
            width: 32 * scaleFactor
            height: 32 * scaleFactor
            checkable: true
            checked: false
            ButtonGroup.group: panButtonGroup3D
            text: "Rotate"

            background: Rectangle {
                implicitHeight: 40 * scaleFactor
                implicitWidth: 40 * scaleFactor
                anchors.fill: rotate
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                width: 26 * scaleFactor
                height: width
//                source: navController.vertical ? "qrc:/Resources/icons/xhdpi/3D.png" : "qrc:/Resources/icons/xhdpi/2D.png"
            }

            onClicked: {
                navController.setRotation();
            }
        } */
}

