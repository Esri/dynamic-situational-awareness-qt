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

        onActiveChanged: {
            if (!active && analysisRoot.visible)
                analysisRoot.visible = false;
        }
    }

    onVisibleChanged: {
        if (!isMobile) {
            toolController.viewshedEnabled = visible;
            toolController.active = visible;
        }
    }

    ButtonGroup {
        id: analysisActiveModeGroup
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

//            Text {
//                width: 80 * scaleFactor
//                text: qsTr("Viewshed Type")
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }

//            ComboBox {
//                id: levelCb
//                anchors {
//                    left: parent.left
//                    right: parent.right
//                }

//                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
//                model: toolController.viewshedTypes
//                currentIndex: toolController.viewshedTypeIndex

//                onCurrentIndexChanged: {
//                    toolController.viewshedTypeIndex = currentIndex;
//                }
//            }

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
                ButtonGroup.group: analysisActiveModeGroup

                onCheckedChanged: {
                    if (checked) {
                        toolController.analysisActiveMode = AnalysisController.AddMapPointViewshed;
                    } else {
                        toolController.analysisActiveMode = AnalysisController.AddMessageFeedViewshed;
                    }
                }
            }

            RadioButton {
                text: "Add Message Feed Viewshed"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: analysisActiveModeGroup
            }
        }

        // Declare the ListView, which will display the list of viewsheds
        ListView {
            id: viewshedList
//            anchors {
//                top: tocRoot.titleBar.bottom
//                left: parent.left
//                right: parent.right
//                bottom: parent.bottom
//                margins: 8 * scaleFactor
//            }
            width: parent.width
            height: 200 * scaleFactor
            interactive: true
            clip: true
            spacing: 5 * scaleFactor
            model: toolController.viewsheds
            highlight: Rectangle {
                radius: 5 * scaleFactor
                color: Material.accent
                opacity: 0.5
            }
            highlightFollowsCurrentItem: isMobile
            highlightMoveVelocity: 10000
            delegate: ListItemDelegate {
                width: parent.width
                height: 40 * scaleFactor
                itemChecked: viewshedVisible
                //imageUrl: imageSourceForGeomType(index)
                //imageVisible: true
                menuIconVisible: true
                mainText: name
                onItemCheckedChanged: viewshedVisible = itemChecked

                Image {
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        margins: 5 * scaleFactor
                    }
                    rotation: 90
                    source: DsaResources.iconMenu
                    height: 32 * scaleFactor
                    width: height

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            viewshedList.currentIndex = index;
                            if (!isMobile) {
                                vehicleMenu.open();
                            } else {
                                if (mobileMenu.isOpen) {
                                    mobileMenu.close();
                                } else {
                                    mobileMenu.open();
                                }
                            }
                        }
                    }

                    // Menu for Vehicle
                    Menu {
                        id: vehicleMenu
                        width: 125 * scaleFactor

                        Column {
                            anchors.margins: 10 * scaleFactor
                            width: parent.width
                            spacing: 10 * scaleFactor
                            leftPadding: 10 * scaleFactor

                            ListLabel {
                                text: qsTr("Zoom to")
                                onTriggered: {
                                    vehicleMenu.close();
                                    tocRoot.closed();
                                    toolController.zoomTo(viewshedList.currentIndex);
                                }
                            }

                            ListLabel {
                                text: qsTr("Remove")
                                onTriggered: {
                                    vehicleMenu.close()
                                    toolController.removeAt(viewshedList.currentIndex);
                                }
                            }

                            CheckBox {
                                text: qsTr("360 Mode")
                                checked: is360Mode
                                onClicked: is360Mode = checked;
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: mobileMenu
            property bool isOpen: y === analysisRoot.y + analysisRoot.height - height
            property int closedY: analysisRoot.y + analysisRoot.height
            property int openY: analysisRoot.y + analysisRoot.height - height - anchors.margins
            anchors {
                left: parent.left
                right: parent.right
                margins: 5 * scaleFactor
            }
            color: "transparent"
            height: analysisRoot.height
            y: closedY

            MouseArea {
                anchors.fill: parent
                onClicked: mobileMenu.close()
            }

            Rectangle {
                anchors {
                    fill: mobileActionColumn
                    margins: -10 * scaleFactor
                }
                color: Material.background
                radius: 10 * scaleFactor
                border {
                    color: Material.primary
                    width: 1 * scaleFactor
                }
            }

            function open() {
                if (y === openY)
                    return;

                viewshedList.highlightFollowsCurrentItem = true;
                animateVertical.from = closedY;
                animateVertical.to = openY;
                animateVertical.start();
            }

            function close() {
                if (y === closedY)
                    return;

                viewshedList.highlightFollowsCurrentItem = false;
                animateVertical.from = openY;
                animateVertical.to = closedY;
                animateVertical.start();
                viewshedList.currentIndex = -1;
            }

            NumberAnimation {
                id: animateVertical
                target: mobileMenu
                properties: "y"
                duration: 250
                easing.type: Easing.OutQuad
            }

            Column {
                id: mobileActionColumn
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    margins: 10 * scaleFactor
                }

                spacing: 5 * scaleFactor

                ListLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Zoom to")
                    onTriggered: {
                        var i = viewshedList.currentIndex;
                        mobileMenu.close();
                        analysisRoot.closed();
                        toolController.zoomTo(i);
                    }
                }

                ListSeparator{}

                ListLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Remove")
                    onTriggered: {
                        toolController.removeAt(viewshedList.currentIndex);
                        mobileMenu.close()
                    }
                }

                ListSeparator {
                    visible: viewshedList.currentIndex !== 0
                }

                ListLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("360 Mode")
                    onTriggered: {
                        mobileMenu.close()
                        analysisRoot.closed();
                    }
                }

                ListSeparator{}

                ListLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Cancel")
                    onTriggered: {
                        mobileMenu.close();
                    }
                }
            }
        }

//        Row {
//            width: parent.width
//            height: 25 * scaleFactor
//            spacing: 5 * scaleFactor

//            OverlayButton {
//                id: button360
//                iconUrl: DsaResources.iconRotate
//                selected: toolController.viewshed360Override
//                onClicked: {
//                    toolController.viewshed360Override = !toolController.viewshed360Override;
//                }
//            }

//            Text {
//                anchors.verticalCenter: button360.verticalCenter
//                text: "360°"
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }
//        }

//        Row {
//            width: parent.width
//            height: 25 * scaleFactor
//            spacing: 5 * scaleFactor

//            CheckBox {
//                id: viewshedVisibleCheckbox
//                text: qsTr("Viewshed Visible")
//                font.pixelSize: 14 * scaleFactor
//                checked: toolController.viewshedVisible
//                contentItem: Label {
//                    text: viewshedVisibleCheckbox.text
//                    font: viewshedVisibleCheckbox.font
//                    verticalAlignment: Text.AlignVCenter
//                    horizontalAlignment: Text.AlignHCenter
//                    color: Material.foreground
//                    leftPadding: viewshedVisibleCheckbox.indicator.width + viewshedVisibleCheckbox.spacing
//                }

//                onCheckedChanged: {
//                    toolController.viewshedVisible = checked;
//                }
//            }
//        }

//        Column {
//            width: parent.width
//            height: 25 * scaleFactor
//            visible: toolController.viewshedEnabled

//            Text {
//                id: distanceRangeLabel
//                width: 80 * scaleFactor
//                text: qsTr("Distance (m)")
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }

//            Row {
//                width: parent.width
//                height: 25 * scaleFactor
//                spacing: 5 * scaleFactor

//                RangeSlider {
//                    anchors.verticalCenter: parent.verticalCenter
//                    orientation: Qt.Horizontal
//                    from: 1
//                    to: 2000
//                    first.value: toolController.minDistance
//                    second.value: toolController.maxDistance
//                    stepSize: 10
//                    snapMode: Slider.SnapAlways
//                    width: parent.width * 0.66

//                    first.onValueChanged: {
//                        toolController.minDistance = first.value;
//                    }

//                    second.onValueChanged: {
//                        toolController.maxDistance = second.value;
//                    }
//                }

//                Text {
//                    id: distanceRangeText
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: 70 * scaleFactor
//                    horizontalAlignment: Text.AlignRight
//                    text: Math.round(toolController.minDistance) + " - " + Math.round(toolController.maxDistance)
//                    color: Material.foreground
//                    font.pixelSize: 14 * scaleFactor
//                }
//            }
//        }

//        Column {
//            width: parent.width
//            height: 25 * scaleFactor
//            visible: toolController.viewshedEnabled && !toolController.viewshed360Override

//            Text {
//                id: horizintalAngleLabel
//                width: 105 * scaleFactor
//                text: qsTr("Horizontal Angle")
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }

//            Row {
//                width: parent.width
//                height: 25 * scaleFactor
//                spacing: 5 * scaleFactor

//                Slider {
//                    id: horizontalAngleSlider
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: parent.width * 0.66
//                    orientation: Qt.Horizontal
//                    from: 0
//                    to: 120
//                    value: toolController.horizontalAngle
//                    stepSize: 1
//                    snapMode: Slider.SnapAlways

//                    onValueChanged: {
//                        toolController.horizontalAngle = value;
//                    }
//                }

//                Text {
//                    id: horizontalAngleText
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: 25 * scaleFactor
//                    horizontalAlignment: Text.AlignRight
//                    text: Math.round(toolController.horizontalAngle) + "°"
//                    color: Material.foreground
//                    font.pixelSize: 14 * scaleFactor
//                }
//            }
//        }

//        Column {
//            width: parent.width
//            height: 25 * scaleFactor
//            visible: toolController.viewshedEnabled && !toolController.viewshed360Override

//            Text {
//                id: verticalAngleLabel
//                width: 105 * scaleFactor
//                text: qsTr("Vertical Angle")
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }

//            Row {
//                width: parent.width
//                height: 25 * scaleFactor
//                spacing: 5 * scaleFactor

//                Slider {
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: parent.width * 0.66
//                    orientation: Qt.Horizontal
//                    from: 0
//                    to: 120
//                    value: toolController.verticalAngle
//                    stepSize: 1
//                    snapMode: Slider.SnapAlways

//                    onValueChanged: {
//                        toolController.verticalAngle = value;
//                    }
//                }

//                Text {
//                    id: veriticalAngleText
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: 25 * scaleFactor
//                    horizontalAlignment: Text.AlignRight
//                    text: Math.round(toolController.verticalAngle) + "°"
//                    color: Material.foreground
//                    font.pixelSize: 14 * scaleFactor
//                }
//            }
//        }

//        Column {
//            width: parent.width
//            height: 25 * scaleFactor
//            visible: toolController.viewshedEnabled && !toolController.viewshed360Override

//            Text {
//                id: headingLabel
//                width: 105 * scaleFactor
//                text: qsTr("Heading")
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }

//            Row {
//                width: parent.width
//                height: 25 * scaleFactor
//                spacing: 5 * scaleFactor

//                Slider {
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: parent.width * 0.66
//                    orientation: Qt.Horizontal
//                    from: 0
//                    to: 359
//                    value: toolController.heading
//                    stepSize: 2
//                    snapMode: Slider.SnapAlways

//                    onValueChanged: {
//                        toolController.heading = value;
//                    }
//                }

//                Text {
//                    id: headingText
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: 25 * scaleFactor
//                    horizontalAlignment: Text.AlignRight
//                    text: Math.round(toolController.heading) + "°"
//                    color: Material.foreground
//                    font.pixelSize: 14 * scaleFactor
//                }
//            }
//        }

//        Column {
//            width: parent.width
//            height: 25 * scaleFactor
//            visible: toolController.viewshedEnabled && !toolController.viewshed360Override

//            Text {
//                id: pitchLabel
//                width: 105 * scaleFactor
//                text: qsTr("Pitch")
//                color: Material.foreground
//                font.pixelSize: 14 * scaleFactor
//            }

//            Row {
//                width: parent.width
//                height: 25 * scaleFactor
//                spacing: 5 * scaleFactor

//                Slider {
//                    anchors.verticalCenter: parent.verticalCenter
//                    width:  parent.width * 0.66
//                    orientation: Qt.Horizontal
//                    from: toolController.viewshedTypeIndex == 0 ? 0 : -90
//                    to: toolController.viewshedTypeIndex == 0 ? 179 : 90
//                    value: toolController.pitch
//                    stepSize: 2
//                    snapMode: Slider.SnapAlways

//                    onValueChanged: {
//                        toolController.pitch = value;
//                    }
//                }

//                Text {
//                    id: pitchText
//                    anchors.verticalCenter: parent.verticalCenter
//                    width: 25 * scaleFactor
//                    horizontalAlignment: Text.AlignRight
//                    text: Math.round(toolController.pitch) + "°"
//                    color: Material.foreground
//                    font.pixelSize: 14 * scaleFactor
//                }
//            }
//        }

//        Button {
//            anchors.horizontalCenter: parent.horizontalCenter
//            text: qsTr("Remove Viewshed")
//            font.pixelSize: 14 * scaleFactor
//            onClicked: toolController.removeViewshed()
//        }
    }
}
