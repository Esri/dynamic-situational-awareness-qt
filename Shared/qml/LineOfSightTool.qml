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
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    id: rootLineOfSight

    LineOfSightController {
        id: toolController

        active: rootLineOfSight.visible
    }

    SecondaryToolbar {
        id: colorPane
        visible: true

        CheckBox {
            id: analysisVisibleCB

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                margins: 5 * scaleFactor
            }
            checked:  toolController.analysisVisible

            onCheckedChanged: {
                if (checked !== toolController.isAnalysisVisible)
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
    }
}
