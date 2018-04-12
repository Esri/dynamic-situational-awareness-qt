// Copyright 2018 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import Esri.DSA 1.0

Item {
    id: aboutRoot
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

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
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Dynamic\nSituational\nAwareness"
                    color: Material.foreground
                    Layout.leftMargin: 30 * scaleFactor
                    font {
                        pixelSize: 24 * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                }

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: DsaResources.iconAppLogo
                    width: 64 * scaleFactor
                    height: width
                    Layout.maximumHeight: height
                    Layout.maximumWidth: width
                    Layout.rightMargin: 30 * scaleFactor
                }
            }

            Rectangle {
                width: parent.width
                height: 1 * scaleFactor
                color: Material.foreground
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Version: 1.0.0"
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
                text: "This example application is built to demonstrate how situational awareness applications can be built with ArcGIS Runtime. It incorporates common workflows such as adding local data sources, sending messages over a secured network, visualizing military symbols, and creating and viewing alerts from those that are in the field."
                color: Material.foreground
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Built with ArcGIS Runtime SDK for Qt 100.2.1"
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
            onClicked: aboutRoot.visible = false;
        }
    }




}
