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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.2

Dialog {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    visible: false
    x: appRoot.width / 2 - width / 2
    y: appRoot.height / 2 - height / 2
    width: 250 * scaleFactor
    standardButtons: Dialog.Yes | Dialog.No

    property alias informativeText: label.text

    Label {
        id: label
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
        wrapMode: Text.Wrap
        width: parent.width
    }
}
