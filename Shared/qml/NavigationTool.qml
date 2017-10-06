
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

Rectangle {
    property int fontSize: 24 * scaleFactor
    property color buttonColor
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    NavigationController {
        id: navController
        objectName: "navigationController"
    }

    RowLayout {
        anchors.fill: parent

        Button {
            id: zoomOutButton
            width: 32 * scaleFactor
            height: 32 * scaleFactor

            background: Rectangle {
                implicitHeight: 40 * scaleFactor
                implicitWidth: 40 * scaleFactor
                anchors.fill: zoomOutButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                width: 26 * scaleFactor
                height: width
                source: "qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png"
            }

            onClicked: {
                navController.zoomOut();
            }
        }

        Button {
            id: zoomInButton
            width: 32 * scaleFactor
            height: 32 * scaleFactor

            background: Rectangle {
                implicitHeight: 40 * scaleFactor
                implicitWidth: 40 * scaleFactor
                anchors.fill: zoomInButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                width: 26 * scaleFactor
                height: width
                source: "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"
            }

            onClicked: {
                navController.zoomIn();
            }
        }

        Button {
            id: birdsEyeButton
            width: 32 * scaleFactor
            height: 32 * scaleFactor

            background: Rectangle {
                implicitHeight: 40 * scaleFactor
                implicitWidth: 40 * scaleFactor
                anchors.fill: birdsEyeButton
                radius: 5 * scaleFactor
                color: buttonColor
            }

            Image {
                anchors.centerIn: parent
                width: 26 * scaleFactor
                height: width
                source: navController.vertical ? "qrc:/Resources/icons/xhdpi/3D.png" : "qrc:/Resources/icons/xhdpi/2D.png"
            }

            onClicked: {
                navController.tilt();
            }
        }
    }
}

