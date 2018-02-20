
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
    id: targetPage

    property bool valid: attributeValueEdit.length > 0
    property string instruction: "Set target"

    function text() {
        return attributeValueEdit.text;
    }

    TextField {
        id: attributeValueEdit
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        color: Material.accent
        width: 256 * scaleFactor
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        font.bold: true
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter

        placeholderText: "<enter value>"
    }
}
