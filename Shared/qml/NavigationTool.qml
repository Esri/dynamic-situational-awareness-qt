
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

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.1
import Esri.DSA 1.0

Item {
    property int fontSize: 24 * scaleFactor
    property color buttonColor
    property real buttonOpacity: 0.75
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    NavigationController {
        id: navController
        objectName: "navigationController"
    }

    ButtonGroup { id: zoomButtonGroup }
//    ButtonGroup { id: panButtonGroup3D }

    height: controlsColumn.height
    width: 32 * scaleFactor
    opacity: buttonOpacity

    Column {
        id: controlsColumn

        Button {
            id: zoomInButton
            width: 32 * scaleFactor
            height: 32 * scaleFactor
            checkable: true
            checked: false
            ButtonGroup.group: zoomButtonGroup

            background: Rectangle {
                anchors.fill: zoomInButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                sourceSize.height: 0.85 * zoomInButton.height
                width: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"
            }

            onClicked: {
                navController.zoomFactor = 2.0;
                navController.zoomIn();
            }
        }

        Rectangle {
            width: 32 * scaleFactor
            height: 1 * scaleFactor
            color: "white"
        }

        Button {
            id: zoomOutButton
            width: 32 * scaleFactor
            height: 32 * scaleFactor
            checkable: true
            checked: false
            ButtonGroup.group: zoomButtonGroup

            background: Rectangle {
                anchors.fill: zoomOutButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                sourceSize.height: 0.85 * zoomOutButton.height
                width: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png"
            }

            onClicked: {
                navController.zoomFactor = 0.5;
                navController.zoomOut();
            }
        }
    }
        // The following buttons have been commented out until we figure out how to get the distance of the camera from the scene view.
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

