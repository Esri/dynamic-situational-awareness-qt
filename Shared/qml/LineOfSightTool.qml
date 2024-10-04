/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    id: rootLineOfSight
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    LineOfSightController {
        id: toolController

        active: rootLineOfSight.visible
    }

    Rectangle {
        id: fill
        anchors {
            top: parent.top
            bottom: viewCount.visible ? viewCount.bottom : overlayNameCB.bottom
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
            if (currentIndex === -1)
                return;

            toolController.selectOverlayIndex(currentIndex)
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
            pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
            italic: true
        }
        color: Material.accent
        verticalAlignment: Text.AlignVCenter

        text: "Visible by <b>" + toolController.visibleByCount + ( toolController.visibleByCount === 1 ? "</b> Observer" :
                                                                                                         "</b> Observers")
    }

    Connections {
        target: toolController

        function onSelectOverlayFailed() {
            overlayNameCB.currentIndex = -1;
        }
    }
}
