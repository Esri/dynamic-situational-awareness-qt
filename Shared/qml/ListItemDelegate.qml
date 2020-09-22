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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1

Item {
    property alias itemChecked: visibleCheckBox.checked
    property url imageUrl
    property bool imageVisible
    property bool imageFrameVisible: true
    property alias checkBoxVisible: visibleCheckBox.visible
    property string mainText
    property bool menuIconVisible: false
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property color imageFrameColor: Material.foreground

    Row {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: 5 * scaleFactor
        }

        id: itemRow
        width: parent.width
        spacing: 3 * scaleFactor

        CheckBox {
            id: visibleCheckBox
            anchors.verticalCenter: parent.verticalCenter
            checked: itemChecked
            onClicked: itemChecked = checked;
        }

        Rectangle {
            id: iconImage
            anchors.verticalCenter: parent.verticalCenter
            radius: 50 * scaleFactor
            width: 24 * scaleFactor
            height: width
            color: imageFrameVisible ? imageFrameColor : "transparent"
            visible: imageVisible

            Image {
                anchors {
                    fill: parent
                    margins: 2 * scaleFactor
                }
                source: imageUrl
            }
        }

        Label {
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideMiddle
            text: mainText
            verticalAlignment: Text.AlignVCenter
            width: menuIconVisible ? parent.width * 0.6 : parent.width
            color: Material.foreground
            font {
                pixelSize: 14 * scaleFactor
                bold: true
                family: DsaStyles.fontFamily
            }
        }
    }

    MouseArea {
        anchors.fill: itemRow
        onClicked: visibleCheckBox.checked = !visibleCheckBox.checked
    }
}
