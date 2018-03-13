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
import QtGraphicalEffects 1.0
import Esri.DSA 1.0

Item {
    id: rootViewshed

    signal myLocationModeSelected

    ViewshedController {
        id: toolController

        active: rootViewshed.visible
        activeMode: ViewshedController.AddLocationViewshed360

        onActiveModeChanged: {
            if (activeMode === ViewshedController.AddMyLocationViewshed360)
                myLocationModeSelected();
        }
    }

    DropShadow {
        anchors.fill: fill
        horizontalOffset: -1 * scaleFactor
        verticalOffset: 1 * scaleFactor
        radius: 8 * scaleFactor
        smooth: true
        samples: 16
        color: "#80000000"
        source: fill
    }

    Rectangle {
        id: fill
        anchors {
            top: parent.top
            bottom: finishToolbar.visible ? finishToolbar.bottom : viewshedTypeToolbar.bottom
            left: parent.left
            right: parent.right
        }
        color: Material.background
    }

    Row {
        id: viewshedTypeToolbar
        visible: true

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: DsaStyles.mainToolbarHeight * scaleFactor
        spacing: 16 * scaleFactor

        Column {
            anchors {
                verticalCenter: parent.verticalCenter
                margins: 8 * scaleFactor
            }
            id: myLocationMode

            OverlayButton {
                anchors.horizontalCenter: parent.horizontalCenter
                iconUrl: DsaResources.iconFollowLocationOff
                selected: toolController.activeMode === ViewshedController.AddMyLocationViewshed360;

                onClicked: {
                    if (toolController.activeMode !== ViewshedController.AddMyLocationViewshed360)
                        toolController.activeMode = ViewshedController.AddMyLocationViewshed360;
                    else
                        toolController.activeMode = ViewshedController.NoActiveMode;
                }
            }

            Text {
                anchors.left: parent.left

                font {
                    family: DsaStyles.fontFamily
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }

                color: Material.accent
                text: "My position"
            }
        }

        Column {
            id: geoElementMode
            anchors {
                verticalCenter: parent.verticalCenter
                margins: 8 * scaleFactor
            }

            OverlayButton {
                iconUrl: DsaResources.iconGps
                selected: toolController.activeMode === ViewshedController.AddGeoElementViewshed360;
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    if (toolController.activeMode !== ViewshedController.AddGeoElementViewshed360)
                        toolController.activeMode = ViewshedController.AddGeoElementViewshed360;
                    else
                        toolController.activeMode = ViewshedController.NoActiveMode;
                }
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font {
                    family: DsaStyles.fontFamily
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }

                color: Material.accent

                text: "Geoelement"
            }
        }

        Column {
            id: coordinateMode
            anchors {
                verticalCenter: parent.verticalCenter
                margins: 8 * scaleFactor
            }

            OverlayButton {
                iconUrl: DsaResources.iconCoordinateConversion
                selected: toolController.activeMode === ViewshedController.AddLocationViewshed360;
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    if (toolController.activeMode !== ViewshedController.AddLocationViewshed360)
                        toolController.activeMode = ViewshedController.AddLocationViewshed360;
                    else
                        toolController.activeMode = ViewshedController.NoActiveMode;
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter

                font {
                    family: DsaStyles.fontFamily
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }

                color: Material.accent

                text: "location"
            }
        }

        CheckBox {
            id: viewshed360ModeCheckbox
            anchors {
                verticalCenter: parent.verticalCenter
                margins: 8 * scaleFactor
            }

            text: qsTr("360 °")
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

    Row {
        id: distanceToolbar
        visible: toolController.activeViewshedEnabled

        anchors {
            top: viewshedTypeToolbar.bottom
            left: parent.left
            right: parent.right
        }

        height: DsaStyles.mainToolbarHeight * scaleFactor

        Column {
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 8 * scaleFactor
            }
            width: parent.width

            Text {
                id: distanceRangeLabel
                anchors{
                    left: parent.left
                    margins: 8 * scaleFactor
                }
                text: qsTr("Distance (m)")
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
                horizontalAlignment: Text.AlignHCenter
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
                        if (Math.round(toolController.activeViewshedMinDistance) !== second.value)
                            toolController.activeViewshedMinDistance = first.value;
                    }

                    second.onValueChanged: {
                        if (Math.round(toolController.activeViewshedMaxDistance) !== second.value)
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
    }

    Row {
        id: normalModeToolbar
        visible: !toolController.activeViewshed360Mode

        anchors {
            top: distanceToolbar.bottom
            left: parent.left
            right: parent.right
        }
        height: DsaStyles.mainToolbarHeight * scaleFactor

        ComboBox {
            id: angleSelector
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 8 * scaleFactor
            }
            width: 100 * scaleFactor
            model: ["Heading", "Horizontal Angle", "Vertical Angle", "Pitch"]
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }

        Column {
            id: headingMode
            visible: angleSelector.currentIndex === 0
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 8 * scaleFactor
            }

            width: parent.width - angleSelector.width

            Text {
                id: headingText
                anchors.horizontalCenter: parent.horizontalCenter
                width: 25 * scaleFactor
                horizontalAlignment: Text.AlignHCenter
                text: Math.round(toolController.activeViewshedHeading) + "°"
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                orientation: Qt.Horizontal
                from: 0
                to: 359
                value: toolController.activeViewshedHeading
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    if (Math.round(toolController.activeViewshedHeading) !== value)
                        toolController.activeViewshedHeading = value;
                }
            }
        }

        Column {
            id: horizontalAngleMode
            visible: angleSelector.currentIndex === 1
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 8 * scaleFactor
            }

            width: parent.width - angleSelector.width

            Text {
                id: horizontalAngleText
                anchors.horizontalCenter: parent.horizontalCenter
                width: 25 * scaleFactor
                horizontalAlignment: Text.AlignHCenter
                text: Math.round(toolController.activeViewshedHorizontalAngle) + "°"
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                orientation: Qt.Horizontal
                from: 0
                to: 120
                value: toolController.activeViewshedHorizontalAngle
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    if (Math.round(toolController.activeViewshedHorizontalAngle) !== value)
                        toolController.activeViewshedHorizontalAngle = value;
                }
            }
        }

        Column {
            id: verticalAngleMode
            visible: angleSelector.currentIndex === 2
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 8 * scaleFactor
            }

            width: parent.width - angleSelector.width

            Text {
                id: verticalAngleText
                anchors.horizontalCenter: parent.horizontalCenter
                width: 25 * scaleFactor
                horizontalAlignment: Text.AlignHCenter
                text: Math.round(toolController.activeViewshedVerticalAngle) + "°"
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                orientation: Qt.Horizontal
                from: 0
                to: 120
                value: toolController.activeViewshedVerticalAngle
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    if (Math.round(toolController.activeViewshedVerticalAngle) !== value)
                        toolController.activeViewshedVerticalAngle = value;
                }
            }
        }

        Column {
            id: pitchMode
            visible: angleSelector.currentIndex === 3
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 8 * scaleFactor
            }

            width: parent.width - angleSelector.width

            Text {
                id: pitchText
                anchors.horizontalCenter: parent.horizontalCenter
                width: 25 * scaleFactor
                horizontalAlignment: Text.AlignHCenter
                text: Math.round(toolController.activeViewshedPitch) + "°"
                color: Material.foreground
                font.pixelSize: 14 * scaleFactor
            }

            Slider {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                orientation: Qt.Horizontal
                from: toolController.activeViewshedMinPitch
                to: toolController.activeViewshedMaxPitch
                value: Math.round(toolController.activeViewshedPitch)
                stepSize: 1
                snapMode: Slider.SnapAlways

                onValueChanged: {
                    if (Math.round(toolController.activeViewshedPitch) !== value)
                        toolController.activeViewshedPitch = value;
                }
            }
        }
    }

    Row {
        id: finishToolbar
        visible: toolController.activeViewshedEnabled

        anchors {
            top: normalModeToolbar.visible ? normalModeToolbar.bottom : distanceToolbar.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        spacing: 16 * scaleFactor
        height: DsaStyles.mainToolbarHeight * scaleFactor

        ToolIcon {
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconComplete
            toolName: "Finish"
            onToolSelected: {
                toolController.finishActiveViewshed();
                toolController.activeMode = ViewshedController.NoActiveMode;
            }
        }

        ToolIcon {
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconClose
            toolName: "Cancel"
            onToolSelected: {
                toolController.removeActiveViewshed();
                toolController.activeMode = ViewshedController.NoActiveMode;
            }
        }
    }
}
