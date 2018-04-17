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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    id: alertsRoot
    title: qsTr("Active Alerts")
    clip: true

    property int hightlightIndex: -1
    property bool isMobile

    AlertListController {
        id: toolController

        onHighlightStopped: hightlightIndex = -1;
    }

    ListView {
        id: alertsView
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: filtersRow.top
            margins: 8 * scaleFactor
        }
        interactive: true
        model: toolController.alertListModel
        clip: true
        spacing: 5 * scaleFactor
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
            itemChecked: true
            imageUrl: level === 1 ?
                          DsaResources.iconAlertLow
                        : ( level === 2 ? DsaResources.iconAlertModerate
                                        : ( level === 3 ? DsaResources.iconAlertHigh
                                                        : DsaResources.iconAlertCritical) )
            imageVisible: true
            imageFrameVisible: false
            checkBoxVisible: false
            mainText: name
            menuIconVisible: true

            Component.onCompleted: {
                if (visible)
                    toolController.setViewed(index);
            }

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
                        alertsView.currentIndex = index;
                        if (!isMobile) {
                            alertMenu.open();
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
                    id: alertMenu
                    width: 125 * scaleFactor

                    Column {
                        anchors.margins: 10 * scaleFactor
                        width: parent.width
                        spacing: 10 * scaleFactor

                        ListLabel {
                            text: "Zoom to"
                            onTriggered: {
                                alertMenu.close();
                                toolController.zoomTo(index);
                            }
                        }

                        ListLabel {
                            text: index === hightlightIndex ? "Highlight (off)" : "Highlight"
                            onTriggered: {
                                alertMenu.close();
                                if (hightlightIndex === index) {
                                    toolController.highlight(index, false);
                                    hightlightIndex = -1;
                                }
                                else {
                                    toolController.highlight(hightlightIndex, false);
                                    toolController.highlight(index, true);
                                    hightlightIndex = index;
                                }
                            }
                        }

                        ListLabel {
                            text: "Dismiss"
                            separatorVisible: false
                            onTriggered: {
                                alertMenu.close();
                                toolController.dismiss(index);
                            }
                        }
                    }
                }
            }
        }
    }

    Row {
        id: filtersRow
        spacing: 8 * scaleFactor
        anchors {
            bottom: levelRow.top
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        Label {
            id: viewCountLabel

            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: "Viewing " + alertsView.count + " of " + toolController.allAlertsCount
            verticalAlignment: Text.AlignVCenter
            color: Material.foreground
            font {
                pixelSize: 14 * scaleFactor
                family: DsaStyles.fontFamily
            }
        }

        Button {
            id: clearAllFiltersButton

            visible: alertsView.count < toolController.allAlertsCount
            anchors.verticalCenter: parent.verticalCenter
            text: "Clear filters"
            font {
                pixelSize: 12 * scaleFactor
                family: DsaStyles.fontFamily
            }

            onClicked: {
                levelFilter.currentIndex = 0;
                toolController.clearAllFilters();
            }
        }
    }

    Row {
        id: levelRow
        spacing: 8 * scaleFactor
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        Text {
            anchors.verticalCenter: levelFilter.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            color: "white"
            text: "Minimum level"
        }

        ComboBox {
            id: levelFilter
            width: 128 * scaleFactor
            model: ["Low", "Medium", "High", "Critical"]
            currentIndex: 0
            onCurrentIndexChanged: toolController.setMinLevel(currentIndex+1);
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }
    }

    Text {
        anchors {
            left: parent.left
            right: parent.right
            top: titleBar.bottom
            margins: 15 * scaleFactor
        }
        visible: alertsView.count === 0
        text: "No active alerts.\n\nSelect 'Conditions' to set up alert queries."
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
    }

    onVisibleChanged: {
        if (!visible)
            return;

        for (var i = 0; i < alertsView.count; ++i)
            toolController.setViewed(i);
    }

    Rectangle {
        id: mobileMenu
        property bool isOpen: y === alertsRoot.y + alertsRoot.height - height
        property int closedY: alertsRoot.y + alertsRoot.height
        property int openY: alertsRoot.y + alertsRoot.height - height - anchors.margins
        anchors {
            left: parent.left
            right: parent.right
            margins: 5 * scaleFactor
        }
        color: "transparent"
        height: alertsRoot.height
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

            alertsView.highlightFollowsCurrentItem = true;
            animateVertical.from = closedY;
            animateVertical.to = openY;
            animateVertical.start();
        }

        function close() {
            if (y === closedY)
                return;

            alertsView.highlightFollowsCurrentItem = false;
            animateVertical.from = openY;
            animateVertical.to = closedY;
            animateVertical.start();
            alertsView.currentIndex = -1;
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
                text: "Zoom to"
                onTriggered: {
                    toolController.zoomTo(alertsView.currentIndex);
                    mobileMenu.close();
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: alertsView.currentIndex === hightlightIndex ? "Highlight (off)" : "Highlight"
                onTriggered: {
                    if (hightlightIndex === alertsView.currentIndex) {
                        toolController.highlight(alertsView.currentIndex, false);
                        hightlightIndex = -1;
                    }
                    else {
                        toolController.highlight(hightlightIndex, false);
                        toolController.highlight(alertsView.currentIndex, true);
                        hightlightIndex = alertsView.currentIndex;
                    }
                    mobileMenu.close();
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Dismiss"
                separatorVisible: false
                onTriggered: {
                    toolController.dismiss(alertsView.currentIndex);
                    mobileMenu.close();
                }
            }
        }
    }

    Timer {
        id: flashTimer
        interval: 500

        property bool highlightOn: true
        running: alertsView.count > 0
        repeat: true
        onTriggered: {
            toolController.flashAll(highlightOn);
            highlightOn = !highlightOn;
        }

        onRunningChanged: {
            if (!running) {
                highlightOn = false;
                toolController.flashAll(highlightOn);
            }
        }
    }
}
