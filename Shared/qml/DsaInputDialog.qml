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
    property alias inputLabel: inputLabelText.text
    property alias inputPlaceholderText: nameText.placeholderText
    property alias userInputText: nameText.text
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property alias titleText: titleLabel.text
    visible: false
    x: appRoot.width / 2 - width / 2
    y: appRoot.height / 2 - height / 2
    width: 250 * scaleFactor
    standardButtons: Dialog.Ok | Dialog.Cancel

    Column {
        width: parent.width
        spacing: 10 * scaleFactor

        Label {
            id: titleLabel
            anchors.horizontalCenter: parent.horizontalCenter
            font {
                family: DsaStyles.fontFamily
                pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
                bold: true
            }
        }

        Row {
            width: parent.width
            spacing: 5 * scaleFactor

            Label {
                id: inputLabelText
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.4
                font {
                    family: DsaStyles.fontFamily
                    pixelSize: 12 * scaleFactor
                }
            }

            TextField {
                id: nameText
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.6
            }
        }
    }

    onOpened: {
        nameText.text = "";
        nameText.focus = true;
    }
}
