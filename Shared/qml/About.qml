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
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1

Item {
    id: aboutRoot
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    MouseArea {
        anchors.fill: parent
        onClicked: mouse.accepted = true
        onPressed: mouse.accepted = true
    }

    Rectangle {
        anchors.centerIn: parent
        width: 300 * scaleFactor
        height: 335 * scaleFactor
        color: Material.primary
        radius: 5 * scaleFactor
        clip: true

        Column {
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            spacing: 10 * scaleFactor

            RowLayout {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 100 * scaleFactor
                spacing: 25 * scaleFactor

                Text {
                    text: "Dynamic\nSituational\nAwareness"
                    color: Material.foreground
                    Layout.alignment: Qt.AlignVCenter
                    Layout.leftMargin: 30 * scaleFactor
                    font {
                        pixelSize: 24 * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                }

                Image {
                    source: DsaResources.iconAppLogo
                    width: 64 * scaleFactor
                    height: width
                    Layout.maximumHeight: height
                    Layout.maximumWidth: width
                    Layout.rightMargin: 30 * scaleFactor
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            Rectangle {
                width: parent.width
                height: 1 * scaleFactor
                color: Material.foreground
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Version: %1".arg(Qt.application.version)
                color: Material.foreground
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                text: "This open source application is built to demonstrate how situational awareness applications can be built with ArcGIS Runtime. It incorporates common workflows such as adding local data sources, sending messages over a secured network, visualizing military symbols, and creating and viewing alerts from those that are in the field."
                color: Material.foreground
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Built with ArcGIS Runtime SDK 100.8 for Qt"
                color: Material.foreground
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
            }
        }

        Button {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                margins: 10 * scaleFactor
            }
            text: "Close"
            font {
                pixelSize: 12 * scaleFactor
                family: DsaStyles.fontFamily
            }
            onClicked: aboutRoot.visible = false;
        }
    }




}
