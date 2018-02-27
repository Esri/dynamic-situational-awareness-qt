
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
    id: conditionPage

    property alias type : typeGroup.checkedButton
    property bool valid: geofenceCB.checked || attributeCB.checked || analysisCB.checked
    property string instruction: "Select type"

    function clear() {
    }

    function text() {
        if (geofenceCB.checked)
            return " GeoFence alert";
        else if (attributeCB.checked)
            return " Attribute alert";
        else if (analysisCB.checked)
            return " Analysis alert";
    }

    property alias isGeoFence: geofenceCB.checked
    property alias isAttribute: attributeCB.checked
    property alias isAnalysis: analysisCB.checked

    Column {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        spacing: 10 * scaleFactor
        leftPadding: 10 * scaleFactor

        ButtonGroup {
            id: typeGroup
        }

        RadioButton {
            id: geofenceCB
            text: "GeoFence"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            checked: true
            ButtonGroup.group: typeGroup
        }

        RadioButton {
            id: attributeCB
            text: "Attribute"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: typeGroup
        }

        RadioButton {
            id: analysisCB
            visible: false
            text: "Analysis"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: typeGroup
        }
    }
}
