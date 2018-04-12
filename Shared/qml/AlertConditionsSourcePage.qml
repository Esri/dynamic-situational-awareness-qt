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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    id: sourcePage

    property bool valid: sourceCb.currentIndex !== -1
    property string instruction: "Select source feed"
    property alias sourceName: sourceCb.currentText
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    function text() {
        return " when objects from " + sourceCb.currentText;
    }

    function clear() {
        sourceCb.currentIndex = -1;
    }

    ComboBox {
        id: sourceCb
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        textRole: "display"
        model: toolController.sourceNames
        currentIndex: 0
    }
}
