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
import QtQuick.Layouts
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Rectangle {
    id: conditionsWizardRoot
    color: Material.primary

    property bool readyToAdd: conditionFrame.currentItem == reviewPage
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    readonly property real wizardButtonsFactoredMargin: 4 * scaleFactor

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
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            italic: true
        }
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
        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        color: Material.accent
        text: conditionFrame.currentItem.instruction
    }

    PageIndicator {
        id: pageIndicator
        anchors {
            top: instructionText.bottom
            horizontalCenter: conditionFrame.horizontalCenter
            margins: 8 * scaleFactor
        }
        count: conditionFrame.count
        currentIndex: conditionFrame.currentIndex
    }

    SwipeView {
        id: conditionFrame
        clip: true
        anchors {
            top: pageIndicator.bottom
            bottom: wizardButtonRow.top
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }
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
            visible: conditionFrame.currentIndex === 0
        }

        // Condition type
        AlertConditionsConditionTypePage {
            id: conditionPage
            visible: conditionFrame.currentIndex === 1
        }

        // Condition Name
        AlertConditionsNamePage {
            id: namePage
            visible: conditionFrame.currentIndex === 2
        }

        // Source feed
        AlertConditionsSourcePage {
            id: sourcePage
            visible: conditionFrame.currentIndex === 3
        }

        Loader {
            id: queryLoader
            visible: conditionFrame.currentIndex === 4
            property bool valid: item ? item.valid : false
            property string instruction: item ? item.instruction : ""

            function text () {
                return item ? item.text() : "";
            }

            function clear() {
                if (item)
                    item.clear();
            }

            source: conditionPage.isSpatial ? "qrc:/qml/AlertConditionsSpatialQueryPage.qml" :
                                               "qrc:/qml/AlertConditionsAttributeQueryPage.qml"
        }

        Loader {
            id: targetLoader
            visible: conditionFrame.currentIndex === 5
            property bool valid: item ? item.valid : false
            property string instruction: item ? item.instruction : ""

            function text () {
                return item ? item.text() : "";
            }

            function clear() {
                if (item)
                    item.clear();
            }

            source: conditionPage.isSpatial ? "qrc:/qml/AlertConditionsSpatialTarget.qml" :
                                               "qrc:/qml/AlertConditionsAttributeTarget.qml"
        }

        // Review
        Item {
            id: reviewPage
            visible: conditionFrame.currentIndex === 6
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
                font {
                    pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
                    bold: true
                }
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                color: Material.foreground
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    RowLayout {
        id: wizardButtonRow
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: wizardButtonsFactoredMargin * 2
        }
        Button {
            id: backButton
            enabled: conditionFrame.currentIndex > 0
            opacity: enabled ? 1.0 : 0.0
            Layout.margins: wizardButtonsFactoredMargin
            Material.roundedScale: Material.NotRounded
            height: nextButton.height
            width: nextButton.width
            text: "Back"
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor * 1.5

            onClicked: conditionFrame.decrementCurrentIndex();
        }

        ToolIcon {
            id: createButton
            Layout.margins: {
                left: 0
                top: wizardButtonsFactoredMargin
                right: wizardButtonsFactoredMargin
                bottom: wizardButtonsFactoredMargin
            }
            enabled: readyToAdd
            opacity: enabled ? 1.0 : 0.5
            iconSource: DsaResources.iconComplete
            toolName: "Create"
            labelColor: Material.accent
            onToolSelected: {
                conditionsWizardRoot.visible = false;
                if (conditionPage.isSpatial) {
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
                                                          targetLoader.item.targetIndex);
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
                conditionFrame.setCurrentIndex(0);
            }
        }

        ToolIcon {
            id: cancelButton
            Layout.margins: {
                left: 0
                top: wizardButtonsFactoredMargin
                right: wizardButtonsFactoredMargin
                bottom: wizardButtonsFactoredMargin
            }
            toolName: "Cancel"
            iconSource: DsaResources.iconClose

            onToolSelected: {
                conditionsWizardRoot.visible = false;
                for (var i = 0; i < conditionFrame.count; ++i)
                    conditionFrame.itemAt(i).clear();
                conditionFrame.setCurrentIndex(0);
            }
        }

        Button {
            id: nextButton
            enabled: conditionFrame.currentIndex < (conditionFrame.count -1) && conditionFrame.currentItem.valid
            opacity: enabled ? 1.0 : 0.0
            Layout.margins: {
                left: 0
                top: wizardButtonsFactoredMargin
                right: wizardButtonsFactoredMargin
                bottom: wizardButtonsFactoredMargin
            }
            Material.roundedScale: Material.NotRounded
            height: 32 * scaleFactor
            width: 64 * scaleFactor
            text: "Next"
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor * 1.5

            onClicked: conditionFrame.incrementCurrentIndex();
        }
    }
}
