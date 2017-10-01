
// Copyright 2016 ESRI
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Esri.DSA 1.0

DsaToolBase {
    id: analysisRoot
    width: 272 * scaleFactor
    title: qsTr("Analysis")

    // Create the controller
    AnalysisController {
        id: toolController
    }

    Column {
        anchors.top: titleBar.bottom
        anchors.margins: 15 * scaleFactor
        spacing: 10 * scaleFactor

        Row {
            anchors.margins: 15 * scaleFactor
            spacing: 10 * scaleFactor

            Text {
                text: qsTr("Analysis Visible")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Switch {
                checked: toolController.analysisVisible

                onCheckedChanged: {
                    toolController.analysisVisible = checked;
                }
            }
        }

        Row {
            spacing: 10 * scaleFactor

            Text {
                text: qsTr("Min Distance")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            TextField {
                text: toolController.minDistance
                color: Material.primary

                onTextChanged: {
                    toolController.minDistance = Number(text)
                }
            }
        }

        Row {
            spacing: 10 * scaleFactor

            Text {
                text: qsTr("Max Distance")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            TextField {
                text: toolController.maxDistance
                color: Material.primary

                onTextChanged: {
                    toolController.maxDistance = Number(text)
                }
            }
        }

        Row {
            spacing: 10 * scaleFactor

            Text {
                text: qsTr("Horizontal Angle")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                orientation: Qt.Horizontal
                from: 1
                to: 180
                value: toolController.horizontalAngle
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    toolController.horizontalAngle = value;
                }
            }
        }

        Row {
            spacing: 10 * scaleFactor

            Text {
                text: qsTr("Vertical Angle")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                orientation: Qt.Horizontal
                from: 1
                to: 180
                value: toolController.verticalAngle
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    toolController.verticalAngle = value;
                }
            }
        }

        Row {
            spacing: 10 * scaleFactor

            Text {
                text: qsTr("Heading")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                orientation: Qt.Horizontal
                from: 1
                to: 360
                value: toolController.heading
                stepSize: 3
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    toolController.heading = value;
                }
            }
        }
    }

}
