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
    id: reportDatePage

    property bool valid: size.length > 0
    property string instruction: "Size of object observed/number of items"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    property alias size: sizeField.text

    function clear() {
        size = "";

        if (visible)
            sizeField.forceActiveFocus();
    }

    function text() {
        return "size:" + sizeField.text;
    }

    onVisibleChanged: {
        if (visible)
            sizeField.forceActiveFocus();
    }

    TextEdit {
        id: sizeField
        clip: true
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        height: parent.height * 0.50
        color: Material.foreground
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        padding: 4 * scaleFactor
        wrapMode: Text.Wrap
        selectByKeyboard: true
        selectByMouse: true

        onTextChanged: {
            if (length > 250)
                remove(250, length -1);
        }

        Rectangle {
            anchors.fill: parent
            border {
                color: Material.accent
                width: 1 * scaleFactor
            }
            color: "transparent"
        }

        Text {
            anchors.centerIn: parent
            visible: sizeField.text.length === 0
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
                italic: true
            }
            opacity: 0.5
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Material.accent

            text: "<enter size>"
        }
    }

    Button {
        anchors {
            top : sizeField.bottom
            right: sizeField.right
            margins: 4 * scaleFactor
        }
        text: "clear"

        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }

        onClicked: clear();
    }
}
