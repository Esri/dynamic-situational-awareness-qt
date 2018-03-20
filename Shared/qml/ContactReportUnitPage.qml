
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

Item {
    id: reportDatePage

    property bool valid: unitName.length > 0
    property string instruction: "Unit making report"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property alias unitName: unitTextField.text

    TextField {
        id: unitTextField
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        color: Material.accent
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: toolController.unitName;
    }
}
