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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

Rectangle {
    id: dsaToolRoot
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property string title: ""
    property alias titleBar: titleBar
    signal closed()
    property string iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png"

    Column {
        id: titleBar
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Rectangle {
            color: Material.primary
            height: 40 * scaleFactor
            width: parent.width

            Text {
                id: titleText
                anchors.centerIn: parent
                text: qsTr(title)
                color: Material.foreground
                font.pixelSize: 20 * scaleFactor
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
                    source: iconSource
                    fillMode: Image.PreserveAspectFit
                }

                onClicked: closed();
            }
        }
    }
}
