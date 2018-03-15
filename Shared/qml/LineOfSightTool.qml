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
import QtQuick.Window 2.2
import QtCharts 2.2
import Esri.DSA 1.0

Item {
    id: rootLineOfSight

    LineOfSightController {
        id: toolController

        active: rootLineOfSight.visible
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
            bottom: chart.bottom//viewCount.visible ? viewCount.bottom : overlayNameCB.bottom
            left: parent.left
            right: parent.right
        }
        color: Material.background
    }

    CheckBox {
        id: analysisVisibleCB

        height: 32 * scaleFactor
        anchors {
            top: parent.top
            left: parent.left
            margins: 5 * scaleFactor
        }
        checked:  toolController.analysisVisible

        onCheckedChanged: {
            if (checked !== toolController.analysisVisible)
                toolController.analysisVisible = checked;
        }
    }

    ComboBox {
        id: overlayNameCB
        anchors {
            verticalCenter: analysisVisibleCB.verticalCenter
            left: analysisVisibleCB.right
            right: clearButton.left
            margins: 5 * scaleFactor
        }
        currentIndex: -1
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        model: toolController.overlayNames
        textRole: "display"

        onCurrentIndexChanged: {
            toolController.selectOverlayIndex(currentIndex);
        }

        Text {
            anchors.centerIn: parent
            visible: overlayNameCB.currentIndex == -1
            text: "<select overlay>"
            color: Material.accent
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                italic: true
            }
        }

        // option 1 - notification bubble
        //        Rectangle {
        //            id: visibleByCount

        //            anchors {
        //                verticalCenter: parent.top
        //                horizontalCenter: parent.right
        //            }

        //            width: toolController.visibleByCount > 0 ?
        //                       (toolController.visibleByCount > 9 ? 16 * scaleFactor
        //                                                          : 12 * scaleFactor)
        //                     : 0
        //            height: width
        //            color: "red"
        //            radius: width * 0.5

        //            Text {
        //                visible: parent.width > 0
        //                anchors.fill: parent
        //                color: "white"
        //                text: toolController.visibleByCount > 99 ? "N" : toolController.visibleByCount
        //                font {
        //                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        //                    bold: true
        //                }
        //                horizontalAlignment: Text.AlignHCenter
        //                verticalAlignment: Text.AlignVCenter
        //            }
        //        }

    }

    OverlayButton {
        id: clearButton
        height: analysisVisibleCB.height
        width: height
        anchors {
            verticalCenter: analysisVisibleCB.verticalCenter
            right: parent.right
            margins: 5 * scaleFactor
        }
        iconUrl: DsaResources.iconClose

        onClicked: {
            toolController.clearAnalysis();
            overlayNameCB.currentIndex = -1;
        }
    }

    // option 2
    Text {
        id: viewCount
        visible: overlayNameCB.currentIndex !== -1
        height: 16 * scaleFactor
        anchors {
            top: overlayNameCB.bottom
            left: overlayNameCB.left
            margins: 5 * scaleFactor
        }

        font {
            family: DsaStyles.fontFamily
            pixelSize: DsaStyles.toolFontPixelSize
            bold: true
        }
        color: Material.foreground
        verticalAlignment: Text.AlignVCenter

        text: "# Visible by: " + toolController.visibleByCount
    }

    // option 3
    ChartView {
        id: chart
        width: parent.width
        height: width
        anchors {
            top: viewCount.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 5 * scaleFactor
        }
        backgroundColor: Material.background
        //          theme: ChartView.ChartThemeBlueIcy
        antialiasing: trues
        legend.visible: false

        PieSeries {
            id: pieSeries
            PieSlice {
                id: vis
                label: "visible"; value: toolController.visibleByCount
            }
            PieSlice {
                id: notVis
                label: "obstructed"; value: toolController.notVisibleByCount

            }
            PieSlice {
                id: unkn
                label: "unknown"; value: toolController.unknownCount

            }
        }

        Row {
            anchors {
                bottom: chart.bottom
                left: chart.left
                right: chart.right
                margins: 5 * scaleFactor
            }
            spacing: 5 * scaleFactor
            Text {
                visible: toolController.visibleByCount > 0
                text: "visible:" + toolController.visibleByCount
                font {
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                }
                color: vis.color
            }

            Text {
                visible: toolController.unknownCount > 0
                text: "unknown:" + toolController.unknownCount
                color: unkn.color
                font {
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                }
            }

            Text {
                visible: toolController.notVisibleByCount > 0
                text: "obstructed:" + toolController.notVisibleByCount
                color: notVis.color
                font {
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    family: DsaStyles.fontFamily
                    bold: true
                }
            }
        }
    }
}
