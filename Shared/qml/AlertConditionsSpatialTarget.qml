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

Item {

    id: targetPage

    property bool valid: targetCB.currentIndex !== -1 &&
                         (featureIdEdit.text.length > 0 || allObjectRb.checked)
    property string instruction: "Set target"
    property alias targetIndex: targetCB.currentIndex
    property int targetFeatureId: singleFeatureRb.checked? Number(featureIdEdit.text) : -1
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

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
