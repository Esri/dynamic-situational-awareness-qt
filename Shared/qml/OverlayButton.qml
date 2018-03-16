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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

Rectangle {
    id: overlayButton
    property alias iconUrl: buttonIcon.source
    property bool selected: false
    property string name: ""
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    signal clicked()

    width: DsaStyles.secondaryIconSize * scaleFactor
    height: width
    color: Material.primary
    radius: 5 * scaleFactor
    opacity: 0.9

    Image {
        id: buttonIcon
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: width
    }

    Rectangle {
        anchors {
            topMargin: 2 * scaleFactor
            horizontalCenter: parent.horizontalCenter
            top: buttonIcon.bottom
        }
        visible: selected
        height: 2 * scaleFactor
        width: parent.width
        color: Material.accent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: overlayButton.clicked()
    }
}
