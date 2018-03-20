
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
    id: manageAlertsRoot
    clip: true
    title: qsTr("Contact Report")

    ContactReportController {
        id: toolController
    }

    Text {
        id: instructionText
        anchors {
            top: titleBar.bottom
            margins: 8 * scaleFactor
            horizontalCenter: parent.horizontalCenter
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        color: Material.accent
        text: reportFrame.currentItem.instruction
    }

    PageIndicator {
        id: pageIndicator
        anchors {
            top: instructionText.bottom
            horizontalCenter: reportFrame.horizontalCenter
            margins: 8 * scaleFactor
        }
        width: nextButton.width
        count: reportFrame.count
        currentIndex: reportFrame.currentIndex
    }

    SwipeView {
        id: reportFrame
        clip: true
        anchors {
            top: pageIndicator.bottom
            bottom: nextButton.top
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }
        currentIndex: 0

        // Date and time of report
        ContactReportDatePage {
            id: reportTimePage
            visible: reportFrame.currentIndex === 0
        }

        // Date and time of report
        ContactReportUnitPage {
            id: unitPage
            visible: reportFrame.currentIndex === 1
        }

        // Size of enemy unit
        ContactReportSizePage {
            id: sizePage
            visible: reportFrame.currentIndex === 2
        }

        // Size of enemy unit
        ContactReportActivityPage {
            id: activityPage
            visible: reportFrame.currentIndex === 3
        }
    }

    Button {
        id: backButton
        visible: reportFrame.currentIndex > 0
        anchors {
            left: reportFrame.left
            verticalCenter: nextButton.verticalCenter
            margins: 8 * scaleFactor
        }
        height: nextButton.height
        width: nextButton.width
        text: "Back"
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

        onClicked: reportFrame.decrementCurrentIndex();
    }

    Button {
        id: nextButton
        visible: reportFrame.currentIndex < (reportFrame.count -1) && reportFrame.currentItem.valid
        anchors {
            right: reportFrame.right
            bottom: parent.bottom
            margins: 16 * scaleFactor
        }
        height: 32 * scaleFactor
        width: 64 * scaleFactor
        text: "Next"
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

        onClicked: reportFrame.incrementCurrentIndex();
    }

}
