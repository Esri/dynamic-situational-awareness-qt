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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property string text: ""
    signal triggered()

    width: 105 * scaleFactor
    height: 32 * scaleFactor

    Label {
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 8 * scaleFactor
        }

        font {
            family: DsaStyles.fontFamily
            pixelSize: 16 * scaleFactor
        }
        color: Material.foreground
        text: parent.text
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.triggered();
        }
    }
}
