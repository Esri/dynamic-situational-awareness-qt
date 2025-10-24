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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    id: levelPage

    property bool valid: levelGroup.checkedButton !== null
    property string instruction: "Select priority"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

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

    component AlertLevelOption: Row {
        property alias iconName: calciteIcon.iconName
        property alias color: calciteIcon.color
        property alias label: radioButton.text
        property alias checked: radioButton.checked
        CalciteIcon {
            id: calciteIcon
            iconName: CalciteIcons.Calcite_exclamation_mark_triangle_f
            height: 24 * scaleFactor
            width: height
        }
        RadioButton {
            id: radioButton
            ButtonGroup.group: levelGroup
            checked: false
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            anchors.verticalCenter: calciteIcon.verticalCenter
        }
    }

    Column {
        id: col
        anchors {
            fill: parent
            margins: 4 * scaleFactor
        }
        spacing: 6 * scaleFactor
        leftPadding: parent.width * 0.25

        AlertLevelOption {
            id: lowCB
            iconName: CalciteIcons.Calcite_information_f
            color: "blue"
            label: "Low"
        }
        AlertLevelOption {
            id: mediumCB
            color: "yellow"
            label: "Moderate"
            checked: true
        }
        AlertLevelOption {
            id: highCB
            color: "orange"
            label: "High"
        }
        AlertLevelOption {
            id: criticalCB
            color: "red"
            label: "Critical"
        }
    }
}
