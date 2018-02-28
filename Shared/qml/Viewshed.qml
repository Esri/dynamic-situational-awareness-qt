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
    property var currentViewshed: null

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
                text: "Add Map Point Viewshed"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: viewshedActiveModeGroup

                onCheckedChanged: {
                    if (checked) {
                        toolController.activeMode = ViewshedController.AddMapPointViewshed;
                    } else {
                        toolController.activeMode = ViewshedController.AddMessageFeedViewshed;
                    }
                }
            }

            RadioButton {
                id: addMessageFeedViewshedButton
                text: "Add Message Feed Viewshed"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: viewshedActiveModeGroup
            }

            ComboBox {
                id: viewshedComboBox
                width: parent.width
                model: toolController.viewsheds
                visible: count > 0
                textRole: "name"
                onCurrentIndexChanged: {
                    currentViewshed = model.at(currentIndex);
                }

                onCountChanged: {
                    if (count == 1) {
                        currentIndex = 0;
                    } else if (currentIndex >= count) {
                        currentIndex = count - 1;
                    }

                    currentViewshed = model.at(currentIndex);
                }
            }

            Row {
                width: parent.width
                height: 25 * scaleFactor
                spacing: 5 * scaleFactor
                visible: currentViewshed !== null

                CheckBox {
                    id: viewshedVisibleCheckbox
                    text: qsTr("Viewshed Visible")
                    font.pixelSize: 14 * scaleFactor
                    checked: currentViewshed ? currentViewshed.visible : false
                    contentItem: Label {
                        text: viewshedVisibleCheckbox.text
                        font: viewshedVisibleCheckbox.font
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: Material.foreground
                        leftPadding: viewshedVisibleCheckbox.indicator.width + viewshedVisibleCheckbox.spacing
                    }

                    onCheckedChanged: {
                        if (currentViewshed) {
                            currentViewshed.visible = checked;
                        }
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
                visible: currentViewshed !== null

                CheckBox {
                    id: viewshed360ModeCheckbox
                    text: qsTr("360 Mode")
                    font.pixelSize: 14 * scaleFactor
                    checked: currentViewshed ? currentViewshed.is360Mode : false
                    contentItem: Label {
                        text: viewshed360ModeCheckbox.text
                        font: viewshed360ModeCheckbox.font
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: Material.foreground
                        leftPadding: viewshed360ModeCheckbox.indicator.width + viewshed360ModeCheckbox.spacing
                    }

                    onCheckedChanged: {
                        if (currentViewshed) {
                            currentViewshed.is360Mode = checked;
                        }
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.active && currentViewshed !== null

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
                        first.value: currentViewshed ? currentViewshed.minDistance : NaN
                        second.value: currentViewshed ? currentViewshed.maxDistance : NaN
                        stepSize: 10
                        snapMode: Slider.SnapAlways
                        width: parent.width * 0.66

                        first.onValueChanged: {
                            if (currentViewshed) {
                                currentViewshed.minDistance = first.value;
                            }
                        }

                        second.onValueChanged: {
                            if (currentViewshed) {
                                currentViewshed.maxDistance = second.value;
                            }
                        }
                    }

                    Text {
                        id: distanceRangeText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 70 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: currentViewshed ? Math.round(currentViewshed.minDistance) + " - " + Math.round(currentViewshed.maxDistance) : ""
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.active && currentViewshed !== null && !currentViewshed.is360Mode

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
                        value: currentViewshed ? currentViewshed.horizontalAngle : NaN
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            if (currentViewshed) {
                                currentViewshed.horizontalAngle = value;
                            }
                        }
                    }

                    Text {
                        id: horizontalAngleText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: currentViewshed ? Math.round(currentViewshed.horizontalAngle) + "°" : ""
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.active && currentViewshed !== null && !currentViewshed.is360Mode

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
                        value: currentViewshed ? currentViewshed.verticalAngle : NaN
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            if (currentViewshed) {
                                currentViewshed.verticalAngle = value;
                            }
                        }
                    }

                    Text {
                        id: veriticalAngleText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: currentViewshed ? Math.round(currentViewshed.verticalAngle) + "°" : ""
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.active && currentViewshed !== null && currentViewshed.headingEnabled

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
                        value: currentViewshed ? currentViewshed.heading : NaN
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            if (currentViewshed) {
                                currentViewshed.heading = value;
                            }
                        }
                    }

                    Text {
                        id: headingText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: currentViewshed ? Math.round(currentViewshed.heading) + "°" : ""
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Column {
                width: parent.width
                height: 25 * scaleFactor
                visible: toolController.active && currentViewshed !== null && currentViewshed.pitchEnabled

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
                        from: !currentViewshed ? NaN : currentViewshed.analysisType === AbstractViewshed.PointViewshed ? 0 : -90
                        to: !currentViewshed ? NaN : currentViewshed.analysisType === AbstractViewshed.PointViewshed ? 179 : 90
                        value: currentViewshed ? currentViewshed.pitch : NaN
                        stepSize: 1
                        snapMode: Slider.SnapAlways

                        onValueChanged: {
                            if (isNaN(to) || isNaN(from)) {
                                return;
                            }

                            if (currentViewshed) {
                                currentViewshed.pitch = value;
                            }
                        }
                    }

                    Text {
                        id: pitchText
                        anchors.verticalCenter: parent.verticalCenter
                        width: 25 * scaleFactor
                        horizontalAlignment: Text.AlignRight
                        text: currentViewshed ? Math.round(currentViewshed.pitch) + "°" : ""
                        color: Material.foreground
                        font.pixelSize: 14 * scaleFactor
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: currentViewshed !== null
                text: qsTr("Remove Viewshed")
                font.pixelSize: 14 * scaleFactor
                onClicked: {
                    if (currentViewshed) {
                        toolController.viewsheds.removeOne(currentViewshed);
                    }
                }
            }
        }
    }
}
