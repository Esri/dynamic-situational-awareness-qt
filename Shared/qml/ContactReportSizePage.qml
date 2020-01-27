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
    id: reportDatePage

    property bool valid: size.length > 0
    property string instruction: "Size of enemy unit"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    property string size: smallButton.checked ? "small" :
                                                (mediumButton.checked ? "medium" : "large")

    function clear() {
    }

    function text() {
        return "size:" + size;
    }

    Column {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        spacing: 10 * scaleFactor
        leftPadding: 10 * scaleFactor

        ButtonGroup {
            id: sizeGroup
        }

        RadioButton {
            id: smallButton
            text: "Small"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            checked: true
            ButtonGroup.group: sizeGroup
        }

        RadioButton {
            id: mediumButton
            text: "Medium"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: sizeGroup
        }

        RadioButton {
            id: largeButton
            visible: true
            text: "Large"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            ButtonGroup.group: sizeGroup
        }
    }
}
