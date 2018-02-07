
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    id: alertsRoot
    width: 272 * scaleFactor
    title: qsTr("Alerts")
    clip: true

    property int hightlightIndex: -1

    AlertListController {
        id: toolController
    }

    Row {
        id: levelRow
        spacing: 8 * scaleFactor
        anchors {
            top: alertsRoot.titleBar.bottom
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

    ListView {
        id: alertsView
        anchors {
            top: levelRow.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        model: toolController.alertListModel
        clip: true
        spacing: 8 * scaleFactor

        delegate: ListItemDelegate {
            width: parent.width
            height: 40 * scaleFactor
            itemChecked: true
            imageUrl: level === 0 ?
                          "qrc:/Resources/icons/xhdpi/warning_blue.png"
                        : ( level === 1 ? "qrc:/Resources/icons/xhdpi/warning_green.png"
                                        : ( level === 2 ? "qrc:/Resources/icons/xhdpi/warning_orange.png"
                                                        : "qrc:/Resources/icons/xhdpi/warning_red.png") )
            imageVisible: true
            checkBoxVisible: false
            mainText: name

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
                        alertMenu.open();
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
                        leftPadding: 10 * scaleFactor

                        ListLabel {
                            text: "Zoom to"
                            onTriggered: {
                                alertMenu.close();
                                toolController.zoomTo(index);
                            }
                        }

                        ListLabel {
                            text: index == hightlightIndex ? "Highlight (off)" : "Highlight"
                            onTriggered: {
                                alertMenu.close()
                                if (hightlightIndex == index) {
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
                            onTriggered: {
                                alertMenu.close()
                                toolController.dismiss(index);
                            }
                        }
                    }
                }
            }
        }
    }

    Text {
        anchors {
            left: parent.left
            right: parent.right
            top: levelRow.bottom
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
    }
}
