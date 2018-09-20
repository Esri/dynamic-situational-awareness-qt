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
import Esri.DSA 1.0

Rectangle {
    id: dsaPanel
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property string title: ""
    property alias titleBar: titleBar
    signal closed()
    property string iconSource: DsaResources.iconClose
    color: Material.primary

    Column {
        id: titleBar
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 5 * scaleFactor
        }

        Rectangle {
            color: Material.primary
            height: 30 * scaleFactor
            width: parent.width

            Text {
                id: titleText
                anchors.centerIn: parent
                text: qsTr(title)
                color: Material.foreground
                font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
            }

            Button {
                id: closeButton
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                    margins: 2 * scaleFactor
                }

                width: height

                background: Rectangle {
                    anchors.fill: closeButton
                    color: Material.primary
                }

                Image {
                    anchors.fill: parent
                    source: iconSource
                    fillMode: Image.PreserveAspectFit
                }

                onClicked: closed();
            }
        }
    }
}
