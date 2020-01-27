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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.0

Item {
    id: conditionPage

    property alias type : typeGroup.checkedButton
    property bool valid: spatialCB.checked || attributeCB.checked || analysisCB.checked
    property string instruction: "Select type"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    function clear() {
    }

    function text() {
        if (spatialCB.checked)
            return " Spatial alert";
        else if (attributeCB.checked)
            return " Attribute alert";
        else if (analysisCB.checked)
            return " Analysis alert";
    }

    property alias isSpatial: spatialCB.checked
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
            id: spatialCB
            text: "Spatial"
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
