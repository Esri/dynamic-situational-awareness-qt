
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

Menu {
    id: conditionsWizardRoot

    property AlertConditionsController controller
    property bool readyToAdd: conditionFrame.currentItem == reviewPage

    Text {
        id: titleRow
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
        color: Material.accent
        text: "Create new Alert Condition"
    }

    Text {
        id: instructionText
        anchors {
            top: titleRow.bottom
            margins: 8 * scaleFactor
            horizontalCenter: parent.horizontalCenter
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: DsaStyles.toolFontPixelSize * scale.Factor
        color: Material.accent
        text: conditionFrame.currentItem.instruction
    }

    SwipeView {
        id: conditionFrame
        anchors {
            top: instructionText.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }
        height: 212 * scaleFactor
        currentIndex: 0

        onCurrentItemChanged: {
            var lastPage = itemAt(currentIndex -1 );
            if (!lastPage || !lastPage.valid)
                decrementCurrentIndex();

            var newText = "";
            for (var i = 0; i < currentIndex; ++i)
                newText += itemAt(i).text();
            summaryText.text = newText;
        }

        // Alert level
        AlertConditionsLevelPage {
            id: levelPage
        }

        // Condition type
        AlertConditionsConditionTypePage {
            id: conditionPage
        }

        // Condition Name
        AlertConditionsNamePage {
            id: namePage
        }

        // Source feed
        AlertConditionsSourcePage {
            id: sourcePage
        }

        Loader {
            id: queryLoader
            property bool valid: item ? item.valid : false
            property string instruction: item ? item.instruction : ""
            function text () {
                return item ? item.text() : "";
            }
            source: conditionPage.isGeoFence ? "qrc:/qml/AlertConditionsGeofenceQueryPage.qml" :
                                               "qrc:/qml/AlertConditionsAttributeQueryPage.qml"
        }

        Loader {
            id: targetLoader
            property bool valid: item ? item.valid : false
            property string instruction: item ? item.instruction : ""
            function text () {
                return item ? item.text() : "";
            }
            source: conditionPage.isGeoFence ? "qrc:/qml/AlertConditionsSpatialTarget.qml" :
                                               "qrc:/qml/AlertConditionsAttributeTarget.qml"
        }

        // Review
        Item {
            id: reviewPage
            property string instruction: "Review new condition"
            property bool valid: true

            function text() {
                return reviewText.text;
            }
            Text {
                id: reviewText
                text: summaryText.text
                width: parent.width * 0.9
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: true
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                color: Material.foreground
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    PageIndicator {
        id: paageIndicator
        anchors {
            top: conditionFrame.bottom
            horizontalCenter: conditionFrame.horizontalCenter
        }

        count: conditionFrame.count
        currentIndex: conditionFrame.currentIndex
    }

    Text {
        id: summaryText
        anchors {
            top: paageIndicator.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 8 * scaleFactor
        }
        width: parent.width * 0.9
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        font.italic: true
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
    }

    Row {
        spacing: 32
        anchors {
            top: summaryText.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 8 * scaleFactor
        }

        ToolIcon {
            id: createButton
            enabled: readyToAdd
            opacity: enabled ? 1.0 : 0.5
            iconSource: DsaResources.iconComplete
            toolName: "Create"
            onToolSelected: {
                conditionsWizardRoot.close();
                if (geofenceCB.checked) {
                    if (withinDistanceRb.checked) {
                        toolController.addWithinDistanceAlert(newAlertName.text,
                                                              levelPage.getLevel(),
                                                              sourceCb.currentText,
                                                              withinDistanceSB.value,
                                                              singleFeatureRb.checked ? Number(featureIdEdit.text) : -1,
                                                                                        targetCB.currentIndex);
                    } else if (withinAreaRb.checked) {
                        toolController.addWithinAreaAlert(newAlertName.text,
                                                          levelPage.getLevel(),
                                                          sourceCb.currentText,
                                                          singleFeatureRb.checked ? Number(featureIdEdit.text) : -1,
                                                                                    targetCB.currentIndex);
                    }
                } else if (attributeCB.checked) {
                    toolController.addAttributeEqualsAlert(newAlertName.text,
                                                           levelPage.getLevel(),
                                                           sourceCb.currentText,
                                                           attributeFieldEdit.text,
                                                           attributeValueEdit.text);
                }

                sourceCb.currentIndex = -1;
                featureIdEdit.text = "";
                targetCB.currentIndex = "";
                attributeFieldEdit.text = "";
                attributeValueEdit.text = "";
                newAlertName.text = "";
            }
        }

        ToolIcon {
            id: cancelButton
            toolName: "Cancel"
            iconSource: DsaResources.iconClose

            onToolSelected: {
                conditionsWizardRoot.close();
            }
        }
    }
}
