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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.2

Item {

    id: targetPage

    property bool valid: targetCB.currentIndex !== -1 &&
                         (featureIdEdit.text.length > 0 || allObjectRb.checked)
    property string instruction: "Set target"
    property alias targetIndex: targetCB.currentIndex
    property int targetFeatureId: singleFeatureRb.checked? Number(featureIdEdit.text) : -1
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    function text() {
        if (allObjectRb.checked) {
            return " object from " + targetCB.currentText;
        }
        else if (singleFeatureRb.checked)
        {
            var msg = " object [%1] from ";
            return msg.arg(featureIdEdit.text) + targetCB.currentText;
        }
    }

    function clear() {
        featureIdEdit.text = "";
        targetCB.currentIndex = -1;
    }

    Column {
        anchors {
            fill: parent
            margins: 16 * scaleFactor
        }

        ButtonGroup {
            id: singleOrAllObjects
        }

        RadioButton {
            id: allObjectRb
            text: qsTr("objects from")
            ButtonGroup.group: singleOrAllObjects
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }

        Row {
            visible: conditionPage.isSpatial || conditionPage.isAnalysis
            spacing: 8 * scaleFactor
            RadioButton {
                id: singleFeatureRb
                text: qsTr("object")
                checked: true
                ButtonGroup.group: singleOrAllObjects
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            }

            TextField {
                id: featureIdEdit
                anchors.verticalCenter: singleFeatureRb.verticalCenter
                validator: IntValidator{}
                color: Material.accent
                font {
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    bold: true
                }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                placeholderText: "<ID>"
                width: 64 * scaleFactor
            }

            Text {
                text: qsTr("from")
                anchors.verticalCenter: singleFeatureRb.verticalCenter
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            OverlayButton {
                id: pickButton
                enabled: singleFeatureRb.checked
                selected: toolController.pickMode
                iconUrl: DsaResources.iconTouch
                opacity: enabled ? 1.0 : 0.8
                anchors.verticalCenter: featureIdEdit.verticalCenter
                onClicked: {
                    toolController.togglePickMode();
                }
            }
        }

        ComboBox {
            id: targetCB
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            width: parent.width * 0.75
            textRole: "display"
            model: toolController.targetNames
            currentIndex: -1
        }
    }

    Connections {
        target: toolController

        onPickedElement: {
            for (var i = 0; i < targetCB.count; ++i) {
                if (targetCB.textAt(i) === overlayName)
                {
                    targetCB.currentIndex = i;
                    featureIdEdit.text = elementId;
                    break;
                }
            }
        }
    }
}
