// Copyright 2017 ESRI
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
import Esri.DSA 1.0

DsaPanel {
    id: analysisRoot
    width: 272 * scaleFactor
    title: qsTr("Analysis")

    property bool isMobile

    // Create the controller
    AnalysisController {
        id: toolController
    }

    onVisibleChanged: {
        if (!isMobile) {
            toolController.viewshedEnabled = visible;
        }
    }

    Column {
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        spacing: 20 * scaleFactor

        Column {
            width: parent.width
            height: 40 * scaleFactor

            Text {
                width: 80 * scaleFactor
                text: qsTr("Viewshed Type")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            ComboBox {
                id: levelCb
                anchors {
                    left: parent.left
                    right: parent.right
                }

                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                model: toolController.viewshedTypes
                currentIndex: toolController.viewshedTypeIndex

                onCurrentIndexChanged: {
                    toolController.viewshedTypeIndex = currentIndex;
                }
            }
        }

        Row {
            width: parent.width
            height: 25 * scaleFactor
            spacing: 5 * scaleFactor
            visible: isMobile

            CheckBox {
                id: viewshedEnabledCheckbox
                text: qsTr("Viewshed Enabled")
                font.pixelSize: 14 * scaleFactor
                checked: toolController.viewshedEnabled
                contentItem: Label {
                    text: viewshedEnabledCheckbox.text
                    font: viewshedEnabledCheckbox.font
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: Material.foreground
                    leftPadding: viewshedEnabledCheckbox.indicator.width + viewshedEnabledCheckbox.spacing
                }

                onCheckedChanged: {
                    toolController.viewshedEnabled = checked;
                }
            }
        }

        Row {
            width: parent.width
            height: 25 * scaleFactor
            spacing: 5 * scaleFactor

            CheckBox {
                id: viewshedVisibleCheckbox
                text: qsTr("Viewshed Visible")
                font.pixelSize: 14 * scaleFactor
                checked: toolController.viewshedVisible
                contentItem: Label {
                    text: viewshedVisibleCheckbox.text
                    font: viewshedVisibleCheckbox.font
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: Material.foreground
                    leftPadding: viewshedVisibleCheckbox.indicator.width + viewshedVisibleCheckbox.spacing
                }

                onCheckedChanged: {
                    toolController.viewshedVisible = checked;
                }
            }
        }

        Column {
            width: parent.width
            height: 25 * scaleFactor
            visible: toolController.viewshedEnabled

            Text {
                id: distanceRangeLabel
                width: 80 * scaleFactor
                text: qsTr("Distance (m)")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor

                RangeSlider {
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: toolController.viewshedEnabled
                    orientation: Qt.Horizontal
                    from: 1
                    to: 2000
                    first.value: toolController.minDistance
                    second.value: toolController.maxDistance
                    stepSize: 10
                    snapMode: Slider.SnapAlways
                    width: parent.width * 0.66

                    first.onValueChanged: {
                        toolController.minDistance = first.value;
                    }

                    second.onValueChanged: {
                        toolController.maxDistance = second.value;
                    }
                }

                Text {
                    id: distanceRangeText
                    anchors.verticalCenter: parent.verticalCenter
                    width: 70 * scaleFactor
                    horizontalAlignment: Text.AlignRight
                    text: Math.round(toolController.minDistance) + " - " + Math.round(toolController.maxDistance)
                    color: Material.foreground
                    font.pixelSize: 14 * scaleFactor
                }
            }
        }

        Column {
            width: parent.width
            height: 25 * scaleFactor
            visible: toolController.viewshedEnabled

            Text {
                id: horizintalAngleLabel
                width: 105 * scaleFactor
                text: qsTr("Horizontal Angle")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor

                Slider {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.66
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
                    anchors.verticalCenter: parent.verticalCenter
                    width: 25 * scaleFactor
                    horizontalAlignment: Text.AlignRight
                    text: Math.round(toolController.horizontalAngle) + "°"
                    color: Material.foreground
                    font.pixelSize: 14 * scaleFactor
                }
            }
        }

        Column {
            width: parent.width
            height: 25 * scaleFactor
            visible: toolController.viewshedEnabled

            Text {
                id: verticalAngleLabel
                width: 105 * scaleFactor
                text: qsTr("Vertical Angle")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor

                Slider {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.66
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
                    anchors.verticalCenter: parent.verticalCenter
                    width: 25 * scaleFactor
                    horizontalAlignment: Text.AlignRight
                    text: Math.round(toolController.verticalAngle) + "°"
                    color: Material.foreground
                    font.pixelSize: 14 * scaleFactor
                }
            }
        }

        Column {
            width: parent.width
            height: 25 * scaleFactor
            visible: toolController.viewshedEnabled

            Text {
                id: headingLabel
                width: 105 * scaleFactor
                text: qsTr("Heading")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor

                Slider {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.66
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
                    anchors.verticalCenter: parent.verticalCenter
                    width: 25 * scaleFactor
                    horizontalAlignment: Text.AlignRight
                    text: Math.round(toolController.heading) + "°"
                    color: Material.foreground
                    font.pixelSize: 14 * scaleFactor
                }
            }
        }

        Column {
            width: parent.width
            height: 25 * scaleFactor
            visible: toolController.viewshedEnabled

            Text {
                id: pitchLabel
                width: 105 * scaleFactor
                text: qsTr("Pitch")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor

                Slider {
                    anchors.verticalCenter: parent.verticalCenter
                    width:  parent.width * 0.66
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
                    anchors.verticalCenter: parent.verticalCenter
                    width: 25 * scaleFactor
                    horizontalAlignment: Text.AlignRight
                    text: Math.round(toolController.pitch) + "°"
                    color: Material.foreground
                    font.pixelSize: 14 * scaleFactor
                }
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Remove Viewshed")
            font.pixelSize: 14 * scaleFactor
            onClicked: toolController.removeViewshed()
        }
    }
}
