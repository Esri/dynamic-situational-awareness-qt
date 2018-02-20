
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
    id: namePage

    property bool valid: newAlertName.length > 0
    property string instruction: "Set name"
    property alias conditionName: newAlertName.text

    function text() {
        return " called " + newAlertName.text;
    }

    function clear() {
        newAlertName.text = "";
    }

    TextField {
        id: newAlertName
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        width: 256 * scaleFactor
        color: Material.accent
        font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        placeholderText: "<enter name>"
    }
}
