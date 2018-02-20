
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

    function openAtPage(pageNumber) {
        conditionFrame.setCurrentIndex(pageNumber);
        open();
    }

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
        text: "Create new Condition"
    }

    Text {
        id: summaryText
        anchors {
            top: titleRow.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 8 * scaleFactor
        }
        width: parent.width * 0.9
        height: 48 * scaleFactor
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        font.italic: true
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        id: instructionText
        anchors {
            top: summaryText.bottom
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
        height: 190 * scaleFactor
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

            function clear() {
                if (item)
                    item.clear();
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

            function clear() {
                if (item)
                    item.clear();
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

            function clear() {
                summaryText.text = "";
            }

            Text {
                id: reviewText
                text: "Create condition?"
                anchors.centerIn: parent
                width: parent.width * 0.75
                font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
                font.bold: true
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                color: Material.foreground
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Button {
        id: backButton
        enabled: conditionFrame.currentIndex > 0
        anchors {
            left: conditionFrame.left
            verticalCenter: pageIndicator.verticalCenter
            margins: 8 * scaleFactor
        }
        height: 32 * scaleFactor
        width: 64 * scaleFactor
        text: "Back"
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

        onClicked: conditionFrame.decrementCurrentIndex();
    }

    PageIndicator {
        id: pageIndicator
        anchors {
            top: conditionFrame.bottom
            horizontalCenter: conditionFrame.horizontalCenter
        }
        width: 64 * scaleFactor
        count: conditionFrame.count
        currentIndex: conditionFrame.currentIndex
    }

    Button {
        enabled: conditionFrame.currentIndex < (conditionFrame.count -1) && conditionFrame.currentItem.valid
        anchors {
            right: conditionFrame.right
            verticalCenter: pageIndicator.verticalCenter
            margins: 16 * scaleFactor
        }
        height: 32 * scaleFactor
        width: 64 * scaleFactor
        text: "Next"
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

        onClicked: conditionFrame.incrementCurrentIndex();
    }

    Row {
        spacing: 32
        anchors {
            top: backButton.bottom
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
                if (conditionPage.isGeoFence) {
                    if (queryLoader.item.isWithinDistance) {
                        toolController.addWithinDistanceAlert(namePage.conditionName,
                                                              levelPage.getLevel(),
                                                              sourcePage.sourceName,
                                                              queryLoader.item.distance,
                                                              targetLoader.item.targetFeatureId,
                                                              targetLoader.item.targetIndex);
                    } else if (queryLoader.item.isWithinArea) {
                        toolController.addWithinAreaAlert(namePage.conditionName,
                                                          levelPage.getLevel(),
                                                          sourcePage.sourceName,
                                                          targetLoader.item.targetFeatureId,
                                                          targetLoader.item.targetInde);
                    }
                } else if (conditionPage.isAttribute) {
                    toolController.addAttributeEqualsAlert(namePage.conditionName,
                                                           levelPage.getLevel(),
                                                           sourcePage.sourceName,
                                                           queryLoader.item.attributeField,
                                                           targetLoader.item.attributeValue);
                }

                for (var i = 0; i < conditionFrame.count; ++i)
                    conditionFrame.itemAt(i).clear();
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
