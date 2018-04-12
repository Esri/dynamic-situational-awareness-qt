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
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0
import Esri.DSA 1.0

Dialog {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    visible: false
    x: appRoot.width / 2 - width / 2
    y: appRoot.height / 2 - height / 2
    width: 250 * scaleFactor
    height: 140 * scaleFactor
    standardButtons: Dialog.Yes | Dialog.No

    property alias informativeText: label.text

    Label {
        id: label
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
        wrapMode: Text.Wrap
        width: parent.width
    }
}
