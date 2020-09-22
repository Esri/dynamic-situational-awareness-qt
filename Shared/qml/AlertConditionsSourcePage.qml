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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1

Item {
    id: sourcePage

    property bool valid: sourceCb.currentIndex !== -1
    property string instruction: "Select source feed"
    property alias sourceName: sourceCb.currentText
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    function text() {
        return " when objects from " + sourceCb.currentText;
    }

    function clear() {
        sourceCb.currentIndex = -1;
    }

    ComboBox {
        id: sourceCb
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        textRole: "display"
        model: toolController.sourceNames
        currentIndex: 0
    }
}
