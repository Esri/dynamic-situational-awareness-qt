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
    id: levelPage

    property bool valid: levelGroup.checkedButton !== null
    property string instruction: "Select priority"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

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

    ButtonGroup {
        id: levelGroup
    }

    Column {
        id: col
        anchors {
            fill: parent
            margins: 4 * scaleFactor
        }
        spacing: 6 * scaleFactor
        leftPadding: parent.width * 0.25

        Row {
            Image {
                id: lowIcon
                source: DsaResources.iconWarningGreen
                height: 28 * scaleFactor
                width: height
            }

            RadioButton {
                id: lowCB

                text: "Low"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: levelGroup
                height: lowIcon.height
                anchors.verticalCenter: lowIcon.verticalCenter
            }
        }

        Row {
            Image {
                id: mediumIcon
                source: DsaResources.iconWarningOrange
                height: lowIcon.height
                width: height
            }

            RadioButton {
                id: mediumCB
                text: "Moderate"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: levelGroup
                height: lowIcon.height
                checked: true
                anchors.verticalCenter: mediumIcon.verticalCenter
            }
        }

        Row {
            Image {
                id: highIcon
                source: DsaResources.iconWarningRed
                height: lowIcon.height
                width: height
            }

            RadioButton {
                id: highCB
                text: "High"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: levelGroup
                height: lowIcon.height
                anchors.verticalCenter: highIcon.verticalCenter
            }
        }

        Row {
            id: critRow
            Image {
                id: criticalIcon
                source: DsaResources.iconWarningRedExclamation
                height: lowIcon.height
                width: height
            }

            RadioButton {
                id: criticalCB
                text: "Critical"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: levelGroup
                anchors.verticalCenter: criticalIcon.verticalCenter
            }
        }
    }
}
