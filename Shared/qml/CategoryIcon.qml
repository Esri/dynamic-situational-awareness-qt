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
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0
import QtQuick.Controls.Material 2.2

Item {
    width: DsaStyles.secondaryIconSize * scaleFactor
    height: width
    property alias iconSource: image.source
    property string categoryName
    property bool selected: false
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    signal categorySelected()

    Column {
        anchors.fill: parent
        spacing: 2 * scaleFactor

        Image {
            id: image
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.75
            height: width
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            height: 2 * scaleFactor
            width: parent.width
            color: Material.accent
            visible: selected
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: categorySelected()
    }
}
