
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

Rectangle {
    signal closed();

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)


    LocationController {
        id: locationController
        enabled: enabledCheckBox.checked;
        simulated: simCheckBox.checked
    }

    anchors{
        top: parent.top
        left: parent.left
    }

    width: parent.width / 4
    color: Material.background

    Rectangle {
        id: titleBar
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
        }
        color: Material.primary
        height: 32 * scaleFactor

        Text {
            id: titleText
            anchors.centerIn: parent
            text: qsTr("Current Location")
            color: Material.foreground
            font.bold: true
        }

        Button {
            id: closeButton

            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: 2 * scaleFactor
            }


            width: height

            background: Rectangle {
                      anchors.fill: closeButton
                      color: Material.primary
                  }

            Image {
                anchors.fill: parent
                source: "qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png"
                fillMode: Image.PreserveAspectFit
            }

            onClicked: closed();
        }
    }

    Column {
        anchors{
            top: titleBar.bottom
            left: parent.left
            right: parent.right
        }
        Row {
            CheckBox {
                id: enabledCheckBox
                text: "Enabled"
            }
        }

        Row {
            CheckBox {
                id: simCheckBox
                text: "Simulated"
            }
        }
    }

    Rectangle {
        id: footerBar
        anchors{
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        color: Material.primary

        height: 32 * scaleFactor

        Label {
            visible: locationController.simulated
            text: locationController.gpxFilePathAsString
            anchors.fill: parent
            wrapMode: Text.WrapAnywhere
            elide: Text.ElideRight
            font.pixelSize: 12 * scaleFactor

        }
    }
}
