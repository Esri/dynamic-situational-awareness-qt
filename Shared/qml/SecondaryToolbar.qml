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
import QtQuick.Controls 1.4 as QtQuick1
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    width : parent.width
    height: DsaStyles.mainToolbarHeight * scaleFactor
    visible: false

    DropShadow {
        anchors.fill: fill
        horizontalOffset: -1 * scaleFactor
        verticalOffset: 1 * scaleFactor
        radius: 8 * scaleFactor
        smooth: true
        samples: 16
        color: "#80000000"
        source: fill
    }

    Rectangle {
        id: fill
        anchors.fill: parent
        color: Material.background
    }
}
