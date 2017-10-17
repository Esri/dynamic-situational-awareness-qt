
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
import QtQuick.Controls 2.2
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
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        spacing: 10 * scaleFactor

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                anchors.left: parent.left
                width: 105 * scaleFactor
                text: qsTr("Viewshed Enabled")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Switch {
                anchors.right: parent.right
                width: 150 * scaleFactor
                checked: toolController.viewshedEnabled

                onCheckedChanged: {
                    toolController.viewshedEnabled = checked;
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                anchors.left: parent.left
                width: parent.width * 0.3
                text: qsTr("Viewshed Visible")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Switch {
                anchors.right: parent.right
                width: 150 * scaleFactor
                checked: toolController.viewshedVisible

                onCheckedChanged: {
                    toolController.viewshedVisible = checked;
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                id: distanceRangeLabel
                anchors.left: parent.left
                width: 80 * scaleFactor
                text: qsTr("Distance (m)")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            RangeSlider {
                anchors {
                    left: distanceRangeLabel.right
                    right: distanceRangeText.left
                    margins: 3 * scaleFactor
                }
                enabled: toolController.viewshedEnabled
                orientation: Qt.Horizontal
                from: 1
                to: 2000
                first.value: toolController.minDistance
                second.value: toolController.maxDistance
                stepSize: 10
                snapMode: Slider.SnapAlways

                first.onValueChanged: {
                    toolController.minDistance = first.value;
                }

                second.onValueChanged: {
                    toolController.maxDistance = second.value;
                }
            }

            Text {
                id: distanceRangeText
                width: 70 * scaleFactor
                anchors.right: parent.right
                horizontalAlignment: Text.AlignRight
                text: Math.round(toolController.minDistance) + " - " + Math.round(toolController.maxDistance)
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                id: horizintalAngleLabel
                anchors.left: parent.left
                width: 105 * scaleFactor
                text: qsTr("Horizontal Angle")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors {
                    left: horizintalAngleLabel.right
                    right: horizontalAngleText.left
                    margins: 3 * scaleFactor
                }
                enabled: toolController.viewshedEnabled
                orientation: Qt.Horizontal
                from: 0
                to: 180
                value: toolController.horizontalAngle
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    toolController.horizontalAngle = value;
                }
            }

            Text {
                id: horizontalAngleText
                width: 25 * scaleFactor
                anchors.right: parent.right
                horizontalAlignment: Text.AlignRight
                text: Math.round(toolController.horizontalAngle) + "°"
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                id: verticalAngleLabel
                anchors.left: parent.left
                width: 105 * scaleFactor
                text: qsTr("Vertical Angle")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors {
                    left: verticalAngleLabel.right
                    right: veriticalAngleText.left
                    margins: 3 * scaleFactor
                }
                enabled: toolController.viewshedEnabled
                orientation: Qt.Horizontal
                from: 0
                to: 180
                value: toolController.verticalAngle
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    toolController.verticalAngle = value;
                }
            }

            Text {
                id: veriticalAngleText
                width: 25 * scaleFactor
                anchors.right: parent.right
                horizontalAlignment: Text.AlignRight
                text: Math.round(toolController.verticalAngle) + "°"
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                id: headingLabel
                anchors.left: parent.left
                width: 105 * scaleFactor
                text: qsTr("Heading")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors {
                    left: headingLabel.right
                    right: headingText.left
                    margins: 3 * scaleFactor
                }
                enabled: toolController.viewshedEnabled
                orientation: Qt.Horizontal
                from: 0
                to: 359
                value: toolController.heading
                stepSize: 2
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    live = true;
                    toolController.heading = value;
                }
            }

            Text {
                id: headingText
                width: 25 * scaleFactor
                anchors.right: parent.right
                horizontalAlignment: Text.AlignRight
                text: Math.round(toolController.heading) + "°"
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Text {
                id: pitchLabel
                anchors.left: parent.left
                width: 105 * scaleFactor
                text: qsTr("Pitch")
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors {
                    left: pitchLabel.right
                    right: pitchText.left
                    margins: 3 * scaleFactor
                }
                enabled: toolController.viewshedEnabled
                orientation: Qt.Horizontal
                from: 0
                to: 179
                value: toolController.pitch
                stepSize: 2
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    toolController.pitch = value;
                }
            }

            Text {
                id: pitchText
                width: 25 * scaleFactor
                anchors.right: parent.right
                horizontalAlignment: Text.AlignRight
                text: Math.round(toolController.pitch) + "°"
                color: Material.primary
                font.pixelSize: 14 * scaleFactor
            }
        }

        Rectangle {
            width: parent.width
            height: 25 * scaleFactor

            Button {
                text: qsTr("Remove Viewshed")
                font.pixelSize: 14 * scaleFactor
                onClicked: toolController.removeViewshed()
            }
        }
    }
}
