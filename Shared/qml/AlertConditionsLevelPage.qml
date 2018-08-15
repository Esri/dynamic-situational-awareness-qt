/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

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
                source: DsaResources.iconAlertLow
                height: 24 * scaleFactor
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
                source: DsaResources.iconAlertModerate
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
                source: DsaResources.iconAlertHigh
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
                source: DsaResources.iconAlertCritical
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
