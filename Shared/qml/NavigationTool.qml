
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

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.1
import Esri.DSA 1.0

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property int fontSize: 24 * scaleFactor
    property int buttonSize: 32 * scaleFactor
    property color buttonColor: Material.primary
    property real buttonOpacity: 0.9

    NavigationController {
        id: navController
        objectName: "navigationController"
    }

    ButtonGroup { id: zoomButtonGroup }

    height: controlsColumn.height
    width: buttonSize
    opacity: buttonOpacity

    Column {
        id: controlsColumn
        spacing: 2 * scaleFactor

        Button {
            id: homeButton
            width: buttonSize
            height: buttonSize

            background: Rectangle {
                anchors.fill: homeButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                sourceSize.height: 0.85 * zoomInButton.height
                width: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png"
            }

            onClicked: {
                navController.zoomToInitialLocation();
            }
        }

        Button {
            id: zoomInButton
            width: buttonSize
            height: buttonSize
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

        Button {
            id: zoomOutButton
            width: buttonSize
            height: buttonSize
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

        Button {
            id: set2DButton
            width: buttonSize
            height: buttonSize
            checkable: true
            checked: false

            background: Rectangle {
                anchors.fill: set2DButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                sourceSize.height: 0.85 * set2DButton.height
                width: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/2D.png"
            }

            onClicked: {
                navController.set2D();
            }
        }

        Button {
            id: panOrRotate
            width: 32 * scaleFactor
            height: 32 * scaleFactor
            checkable: true
            checked: false

            background: Rectangle {
                implicitHeight: 40 * scaleFactor
                implicitWidth: 40 * scaleFactor
                anchors.fill: panOrRotate
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                sourceSize.height: 0.85 * zoomInButton.height
                width: sourceSize.height
                source: panOrRotate.checked ? "qrc:/Resources/icons/xhdpi/ic_menu_refresh_dark.png" :
                                              "qrc:/Resources/icons/xhdpi/ic_menu_drag_dark.png"
            }

            onCheckedChanged: {
                if (checked)
                    navController.setRotation();
                else
                    navController.pan();
            }
        }
    }
}

