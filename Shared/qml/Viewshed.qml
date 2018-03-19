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
            margins: 8 * scaleFactor
        }
        height: myLocationMode.height + (anchors.margins * 2)
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
                color: "transparent"
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

                color: Material.foreground
                text: "current\nposition"
            }
        }

        Column {
            id: feedsMode
            anchors {
                verticalCenter: parent.verticalCenter
                margins: 8 * scaleFactor
            }

            OverlayButton {
                iconUrl: DsaResources.iconGps
                selected: toolController.activeMode === ViewshedController.AddGeoElementViewshed360;
                anchors.horizontalCenter: parent.horizontalCenter
                color: "transparent"
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

                color: Material.foreground

                text: "track\nposition"
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
                color: "transparent"
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

                color: Material.foreground

                text: "follow\nmouse"
            }
        }

        Column {
            id: mode360
            anchors {
                verticalCenter: parent.verticalCenter
                margins: 8 * scaleFactor
            }

            Switch {
                id: control
                anchors.horizontalCenter: parent.horizontalCenter
                width: DsaStyles.secondaryIconSize * scaleFactor
                height: width

                checked: toolController.activeViewshed360Mode

                indicator: Rectangle {
                    implicitWidth: parent.width * 0.75
                    implicitHeight: parent.height * 0.4
                    x: control.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 13
                    color: control.checked ? Material.accent : "#ffffff"
                    border.color: Material.background

                    Rectangle {
                        x: control.checked ? parent.width - width : 0
                        width: height
                        height: parent.height
                        radius: height
                        color: control.down ? "#cccccc" : "#ffffff"
                        border.color: Material.background
                    }
                }

                onCheckedChanged: {
                    toolController.activeViewshed360Mode = checked;
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter

                font {
                    family: DsaStyles.fontFamily
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }

                color: Material.foreground
                text: qsTr("360 °")
            }
        }
    }

    Text {
        id: distanceRangeLabel
        visible: toolController.activeViewshedEnabled
        anchors{
            left: parent.left
            top: viewshedTypeToolbar.bottom
            margins: 8 * scaleFactor
        }
        text: qsTr("Distance\n(meters)")
        color: Material.foreground
        font {
            family: DsaStyles.fontFamily
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }
        horizontalAlignment: Text.AlignHCenter
    }

    RangeSlider {
        visible: toolController.activeViewshedEnabled
        anchors{
            left: distanceRangeLabel.right
            right: distanceRangeText.left
            margins: 8 * scaleFactor
            verticalCenter: distanceRangeLabel.verticalCenter
        }
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
        visible: toolController.activeViewshedEnabled
        anchors {
            verticalCenter: distanceRangeLabel.verticalCenter
            right: parent.right
            margins: 8 * scaleFactor
        }
        horizontalAlignment: Text.AlignRight
        text: Math.round(toolController.activeViewshedMinDistance) + " - " + Math.round(toolController.activeViewshedMaxDistance)
        color: Material.foreground
        font {
            family: DsaStyles.fontFamily
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }
    }

    ComboBox {
        id: angleSelector
        visible: !toolController.activeViewshed360Mode
        anchors {
            top: distanceRangeLabel.bottom
            left: parent.left
            margins: 8 * scaleFactor
        }
        height: 32 * scaleFactor
        width: 100 * scaleFactor

        property var allAnglesModel: ["Heading", "Horizontal Angle", "Vertical Angle", "Pitch"]
        property var trackModel: ["Horizontal Angle", "Vertical Angle"]

        model: toolController.activeMode === ViewshedController.AddMyLocationViewshed360 ? trackModel : allAnglesModel
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

        background: Rectangle {
            color: Material.primary
            border.color: Material.accent
            border.width: 1 * scaleFactor
            radius: 2 * scaleFactor
            anchors.fill: parent
        }

        delegate: ItemDelegate {
            width: angleSelector.width
            contentItem: Text {
                text: modelData
                color: highlighted ? Material.accent : Material.foreground
                font: angleSelector.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }
            highlighted: angleSelector.highlightedIndex === index
        }
    }

    Slider {
        id: angleSlider
        visible: angleSelector.visible
        anchors {
            left: angleSelector.right
            right: angleText.left
            verticalCenter: angleSelector.verticalCenter
            margins: 8 * scaleFactor
        }
        orientation: Qt.Horizontal
        from: getMinAngle()
        to: getMaxAngle()
        value: getValue()
        stepSize: 1
        snapMode: Slider.SnapAlways

        onValueChanged: {
            if (angleSelector.currentText === "Heading") {
                if (Math.round(toolController.activeViewshedHeading) !== value)
                    toolController.activeViewshedHeading = value;
            }
            if (angleSelector.currentText === "Horizontal Angle") {
                if (Math.round(toolController.activeViewshedHorizontalAngle) !== value)
                    toolController.activeViewshedHorizontalAngle = value;
            }
            if (angleSelector.currentText === "Vertical Angle") {
                if (Math.round(toolController.activeViewshedVerticalAngle) !== value)
                    toolController.activeViewshedVerticalAngle = value;
            }
            if (angleSelector.currentText === "Pitch") {
                if (Math.round(toolController.activeViewshedPitch) !== value)
                    toolController.activeViewshedPitch = value;
            }
        }

        function getMinAngle() {
            if (angleSelector.currentText === "Pitch")
                return toolController.activeViewshedMinPitch;

            return 0;
        }

        function getMaxAngle() {
            if (angleSelector.currentText === "Heading")
                return 359;
            else if (angleSelector.currentText === "Pitch")
                return toolController.activeViewshedMaxPitch;

            return 120;
        }

        function getValue() {
            if (angleSelector.currentText === "Heading")
                return Math.round(toolController.activeViewshedHeading);
            if (angleSelector.currentText === "Horizontal Angle")
                return Math.round(toolController.activeViewshedHorizontalAngle);
            if (angleSelector.currentText === "Vertical Angle")
                return Math.round(toolController.activeViewshedVerticalAngle);
            if (angleSelector.currentText === "Pitch")
                return Math.round(toolController.activeViewshedPitch);
        }
    }

    Text {
        id: angleText
        visible: angleSelector.visible
        anchors {
            right: parent.right
            verticalCenter: angleSelector.verticalCenter
            margins: 8 * scaleFactor
        }
        width: 25 * scaleFactor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        text: getAngleText()
        color: Material.foreground
        font {
            family: DsaStyles.fontFamily
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }

        function getAngleText() {
            if (angleSelector.currentIndex === 0)
                return Math.round(toolController.activeViewshedHeading) + "°";
            else if (angleSelector.currentIndex === 1)
                return Math.round(toolController.activeViewshedHorizontalAngle) + "°";
            else if (angleSelector.currentIndex === 2)
                return Math.round(toolController.activeViewshedVerticalAngle) + "°";
            else if (angleSelector.currentIndex === 3)
                return Math.round(toolController.activeViewshedPitch) + "°";
        }
    }

    Row {
        id: finishToolbar
        visible: toolController.activeViewshedEnabled

        anchors {
            top: angleSelector.visible ? angleSelector.bottom : distanceRangeLabel.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        spacing: 16 * scaleFactor
        height: finishIcon.height + (anchors.margins * 2)

        ToolIcon {
            id: finishIcon
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
