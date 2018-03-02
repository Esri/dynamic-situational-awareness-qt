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
    id: viewshedRoot
    width: 272 * scaleFactor
    title: qsTr("Viewshed")

    property bool isMobile

    // Create the controller
    ViewshedController {
        id: toolController

        onActiveChanged: {
            if (!active && viewshedRoot.visible)
                viewshedRoot.visible = false;
        }
    }

    onVisibleChanged: {
        if (!isMobile) {
            toolController.active = visible;
        }
    }

    ButtonGroup {
        id: viewshedActiveModeGroup
    }

    Column {
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }

        Column {
            width: parent.width
            spacing: 5 * scaleFactor

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor
                visible: isMobile

                CheckBox {
                    id: viewshedEnabledCheckbox
                    text: qsTr("Viewshed Enabled")
                    font.pixelSize: 14 * scaleFactor
                    checked: toolController.active
                    contentItem: Label {
                        text: viewshedEnabledCheckbox.text
                        font: viewshedEnabledCheckbox.font
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: Material.foreground
                        leftPadding: viewshedEnabledCheckbox.indicator.width + viewshedEnabledCheckbox.spacing
                    }

                    onCheckedChanged: {
                        toolController.active = checked;
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !toolController.locationDisplayViewshedActive
                text: qsTr("Add Location Display Viewshed")
                font.pixelSize: 14 * scaleFactor
                onClicked: toolController.addLocationDisplayViewshed()
            }

            RadioButton {
                text: "Add Location Viewshed"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: viewshedActiveModeGroup

                onCheckedChanged: {
                    if (checked) {
                        toolController.activeMode = ViewshedController.AddLocationViewshed360;
                    }
                }
            }

            RadioButton {
                text: "Add GeoElement Viewshed"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: viewshedActiveModeGroup

                onCheckedChanged: {
                    if (checked) {
                        toolController.activeMode = ViewshedController.AddGeoElementViewshed360;
                    }
                }
            }

            ComboBox {
                id: viewshedComboBox
                width: parent.width
                model: toolController.viewsheds
                visible: count > 0
                textRole: "name"
                currentIndex: toolController.activeViewshedIndex
                onCurrentIndexChanged: {
                    toolController.activeViewshedIndex = currentIndex;
                }
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor
                visible: toolController.activeViewshedEnabled

                CheckBox {
                    id: viewshedVisibleCheckbox
                    text: qsTr("Viewshed Visible")
                    font.pixelSize: 14 * scaleFactor
                    checked: toolController.activeViewshedVisible
                    contentItem: Label {
                        text: viewshedVisibleCheckbox.text
                        font: viewshedVisibleCheckbox.font
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: Material.foreground
                        leftPadding: viewshedVisibleCheckbox.indicator.width + viewshedVisibleCheckbox.spacing
                    }

                    onCheckedChanged: {
                        toolController.activeViewshedVisible = checked;
                    }
                }
            }
        }

        Column {
            width: parent.width
            spacing: 20 * scaleFactor

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor
                visible: toolController.activeViewshedEnabled

                CheckBox {
                    id: viewshed360ModeCheckbox
                    text: qsTr("360 Mode")
                    font.pixelSize: 14 * scaleFactor
                    checked: toolController.activeViewshed360Mode
                    contentItem: Label {
                        text: viewshed360ModeCheckbox.text
                        font: viewshed360ModeCheckbox.font
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: Material.foreground
                        leftPadding: viewshed360ModeCheckbox.indicator.width + viewshed360ModeCheckbox.spacing
                    }

                    onCheckedChanged: {
                        toolController.activeViewshed360Mode = checked;
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.activeViewshedEnabled

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
                        orientation: Qt.Horizontal
                        from: 1
                        to: 2000
                        first.value: toolController.activeViewshedMinDistance
                        second.value: toolController.activeViewshedMaxDistance
                        stepSize: 10
                        snapMode: Slider.SnapAlways
                        width: parent.width * 0.66

                        first.onValueChanged: {
                            toolController.activeViewshedMinDistance = first.value;
                        }

                        second.onValueChanged: {
                            toolController.activeViewshedMaxDistance = second.value;
                        }
                    }

                    Text {
                        id: distanceRangeText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 70 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: Math.round(toolController.activeViewshedMinDistance) + " - " + Math.round(toolController.activeViewshedMaxDistance)
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.activeViewshedEnabled && !toolController.activeViewshed360Mode

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
                        id: horizontalAngleSlider
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width * 0.66
                        orientation: Qt.Horizontal
                        from: 0
                        to: 120
                        value: toolController.activeViewshedHorizontalAngle
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            toolController.activeViewshedHorizontalAngle = value;
                        }
                    }

                    Text {
                        id: horizontalAngleText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: Math.round(toolController.activeViewshedHorizontalAngle) + "°"
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.activeViewshedEnabled && !toolController.activeViewshed360Mode

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
                        orientation: Qt.Horizontal
                        from: 0
                        to: 120
                        value: toolController.activeViewshedVerticalAngle
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            toolController.activeViewshedVerticalAngle = value;
                        }
                    }

                    Text {
                        id: veriticalAngleText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: Math.round(toolController.activeViewshedVerticalAngle) + "°"
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.activeViewshedEnabled && toolController.activeViewshedHeadingEnabled

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
                        orientation: Qt.Horizontal
                        from: 0
                        to: 359
                        value: toolController.activeViewshedHeading
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            toolController.activeViewshedHeading = value;
                        }
                    }

                    Text {
                        id: headingText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: Math.round(toolController.activeViewshedHeading) + "°"
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.activeViewshedEnabled && toolController.activeViewshedPitchEnabled

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
                        orientation: Qt.Horizontal
                        from: toolController.activeViewshedMinPitch
                        to: toolController.activeViewshedMaxPitch
                        value: toolController.activeViewshedPitch
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            if (isNaN(to) || isNaN(from)) {
                                return;
                            }

                            toolController.activeViewshedPitch = value;
                        }
                    }

                    Text {
                        id: pitchText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: Math.round(toolController.activeViewshedPitch) + "°"
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.activeViewshedEnabled && toolController.activeViewshedOffsetZEnabled

                Text {
                    id: offsetZLabel
                    width: 105 * scaleFactor
                    text: qsTr("Offset Z")
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
                        orientation: Qt.Horizontal
                        from: -30
                        to: 30
                        value: toolController.activeViewshedOffsetZ
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            toolController.activeViewshedOffsetZ = value;
                        }
                    }

                    Text {
                        id: offsetZText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: Math.round(toolController.activeViewshedOffsetZ)
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: toolController.activeViewshedEnabled
                text: qsTr("Remove Viewshed")
                font.pixelSize: 14 * scaleFactor
                onClicked: {
                    toolController.removeActiveViewshed();
                }
            }
        }
    }
}
