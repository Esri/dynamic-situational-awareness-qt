
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
    id: levelPage

    property bool valid: levelGroup.checkedButton !== null
    property string instruction: "Select priority"

    function getLevel() {
        if (lowCB.checked)
            return 1;
        else if (mediumCB.checked)
            return 2;
        else if (highCB.checked)
            return 3;
        else if (criticalCB.checked)
            return 4;
    }

    function text() {
        var prefix = "Create a new ";
        if (lowCB.checked)
            return prefix + "low priority";
        else if (mediumCB.checked)
            return prefix + "medium priority";
        else if (highCB.checked)
            return prefix + "high priority";
        else if (criticalCB.checked)
            return prefix + "critical priority";
    }

    function clear() {

    }

    Grid {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        columns: 2
        rows: 4
        spacing: 10 * scaleFactor
        leftPadding: 10 * scaleFactor

        Image {
            source: DsaResources.iconWarningGreen
        }

        ButtonGroup {
            id: levelGroup
        }

        RadioButton {
            id: lowCB
            text: "Low"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: levelGroup
        }

        Image {
            source: DsaResources.iconWarningOrange
        }

        RadioButton {
            id: mediumCB
            text: "Medium"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: levelGroup
        }

        Image {
            source: DsaResources.iconWarningRed
        }

        RadioButton {
            id: highCB
            text: "High"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: levelGroup
        }

        Image {
            source: DsaResources.iconWarningRedExclamation
        }

        RadioButton {
            id: criticalCB
            text: "Critical"
            ButtonGroup.group: levelGroup
        }
    }
}
