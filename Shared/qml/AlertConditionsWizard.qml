
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

    property var pageInstructions: [
        "Select priority",
        "Set name",
        "Select type",
        "Select source feed",
        "Set query",
        "Set target",
        "Review new condition"]

    property bool readyToAdd: geofenceReadyToAdd || attributeReadyToAdd || analysisReadyToAdd

    property bool geofenceReadyToAdd:  geofenceCB.checked && (levelGroup.checkedButton !== null &&
                                                              sourceCb.currentIndex !== -1 &&
                                                              targetCB.currentIndex !== -1 &&
                                                              (featureIdEdit.text.length > 0 || allObjectRb.checked))
    property bool attributeReadyToAdd: attributeCB.checked && (levelGroup.checkedButton !== null &&
                                                               sourceCb.currentIndex !== -1 &&
                                                               attributeFieldEdit.length > 0 &&
                                                               attributeValueEdit.length > 0)
    property bool analysisReadyToAdd: analysisCB.checked && (levelGroup.checkedButton !== null &&
                                                             sourceCb.currentIndex !== -1 &&
                                                             targetCB.currentIndex !== -1 &&
                                                             (featureIdEdit.text.length > 0 || allObjectRb.checked))

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
        text: pageInstructions[conditionFrame.currentIndex]
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

        ButtonGroup {
            id: levelGroup
        }

        onCurrentItemChanged: {
            if (currentItem === conditionPage)
            {
                if (levelGroup.checkedButton === null)
                    decrementCurrentIndex();
                else
                    summaryText.text = levelPage.getLevelText();

            }
            else if (currentItem === namePage)
            {
                summaryText.text = levelPage.getLevelText() + conditionPage.getConditionTypeText() + namePage.getNameText();
            }
            else if (currentItem === sourcePage)
            {
                if (newAlertName.length === 0)
                    decrementCurrentIndex();
                else
                    summaryText.text = levelPage.getLevelText() + conditionPage.getConditionTypeText() + namePage.getNameText();
            }
            else if (currentItem === queryPage)
            {
                if (sourceCb.currentIndex === -1)
                    decrementCurrentIndex();
                else
                    summaryText.text = levelPage.getLevelText() + conditionPage.getConditionTypeText() + namePage.getNameText() +
                            sourcePage.getSourceText();

            }
            else if (currentItem === targetPage)
            {
                if (sourceCb.currentIndex === -1)
                    decrementCurrentIndex();
                else
                    summaryText.text = levelPage.getLevelText() + conditionPage.getConditionTypeText() + namePage.getNameText() +
                            sourcePage.getSourceText() + queryPage.getQueryText();
            }
            else if (currentItem === reviewPage)
            {
                if (sourceCb.currentIndex === -1)
                    decrementCurrentIndex();
                else
                    summaryText.text = levelPage.getLevelText() + conditionPage.getConditionTypeText() + namePage.getNameText() +
                            sourcePage.getSourceText() + queryPage.getQueryText() + targetPage.getTargetText();
            }
        }

        // Alert level
        Item {
            id: levelPage
            function getLevelText() {
                var prefix = "Create a new ";
                if (lowCB.checked)
                    return prefix + "low priority";
                else if (mediumCB.checked)
                    return prefix + "medium priority";
                else if (highCB.checked)
                    return prefix + "high priority";
                else if (criticalCB.checked)
                    return prefix + "critical priority";
            }

            Grid {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                columns: 2
                rows: 4
                spacing: 10 * scaleFactor
                leftPadding: 10 * scaleFactor

                Image {
                    source: DsaResources.iconWarningGreen
                }

                RadioButton {
                    id: lowCB
                    text: "Low"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    ButtonGroup.group: levelGroup
                }

                Image {
                    source: DsaResources.iconWarningOrange
                }

                RadioButton {
                    id: mediumCB
                    text: "Medium"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    ButtonGroup.group: levelGroup
                }

                Image {
                    source: DsaResources.iconWarningRed
                }

                RadioButton {
                    id: highCB
                    text: "High"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    ButtonGroup.group: levelGroup
                }

                Image {
                    source: DsaResources.iconWarningRedExclamation
                }

                RadioButton {
                    id: criticalCB
                    text: "Critical"
                    ButtonGroup.group: levelGroup
                }
            }
        }

        // Condition type
        Item {
            id: conditionPage

            function getConditionTypeText() {
                if (geofenceCB.checked)
                    return " GeoFence alert";
                else if(attributeCB.checked)
                    return " Attribute alert";
                else if(analysisCB.checked)
                    return " Analysis alert";
            }

            Column {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                spacing: 10 * scaleFactor
                leftPadding: 10 * scaleFactor

                RadioButton {
                    id: geofenceCB
                    text: "GeoFence"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    checked: true
                }

                RadioButton {
                    id: attributeCB
                    text: "Attribute"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }

                RadioButton {
                    id: analysisCB
                    enabled: false
                    text: "Analysis"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }
            }
        }

        // Condition Name
        Item {
            id: namePage

            function getNameText() {
                return " called " + newAlertName.text;
            }

            TextField {
                id: newAlertName
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                width: 256 * scaleFactor
                color: Material.accent
                font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                placeholderText: "<enter name>"
            }
        }

        // Source feed
        Item {
            id: sourcePage
            function getSourceText() {
                return " when objects from " + sourceCb.currentText;
            }

            ComboBox {
                id: sourceCb
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                width: 256 * scaleFactor
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                textRole: "display"
                model: controller.sourceNames
                currentIndex: 0
            }
        }

        // Query
        Item {
            id: queryPage
            function getQueryText() {
                if (withinAreaRb.visible && withinAreaRb.checked) {
                    return " are within area of";
                }
                else if (withinDistanceRb.visible && withinDistanceRb.checked) {
                    return " are within " + withinDistanceSB.value + " m of";
                }
                else if (attributeFieldEdit.visible) {
                    return "have attrribute " + attributeFieldEdit.text + " = ";
                }
            }

            Column {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                ButtonGroup {
                    id: spatialOperationGroup
                }

                RadioButton {
                    id: withinAreaRb
                    visible: geofenceCB.checked
                    text: "are within area of"
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    checked: true
                    ButtonGroup.group: spatialOperationGroup
                }

                Row {
                    visible: geofenceCB.checked
                    spacing: 8 * scaleFactor
                    RadioButton {
                        id: withinDistanceRb
                        text: checked ? "are within" : "are within distance of"
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        ButtonGroup.group: spatialOperationGroup
                    }

                    SpinBox {
                        id: withinDistanceSB
                        anchors.verticalCenter: withinDistanceRb.verticalCenter
                        visible: withinDistanceRb.checked
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        editable: true
                        value: 50
                        from: 0
                        to: 9999
                    }

                    Text {
                        anchors.verticalCenter: withinDistanceRb.verticalCenter
                        visible: withinDistanceRb.checked
                        text: qsTr("m of")
                        color: Material.foreground
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Grid {
                    rows: 2
                    columns: 2
                    visible: attributeCB.checked
                    spacing: 8 * scaleFactor

                    Text {
                        id: fieldLabel
                        text: qsTr("have attribute")
                        color: Material.foreground
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextField {
                        id: attributeFieldEdit
                        color: Material.accent
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        font.bold: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter

                        placeholderText: "<enter attribute name>"
                    }

                    Text {
                        text: qsTr("equal to")
                        color: Material.foreground
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Text {
                    id: visibleLabel
                    visible: analysisCB.checked
                    text: qsTr("can be seen by")
                    color: Material.foreground
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        // Target
        Item {

            id: targetPage

            function getTargetText() {
                if (allObjectRb.visible && allObjectRb.checked )
                    return " object from " + targetCB.currentText;
                else if (singleFeatureRb.visible && singleFeatureRb.checked)
                    return "object [" + featureIdEdit.text + "] from " + targetCB.currentText;
                else if (attributeValueEdit.visible)
                    return attributeValueEdit.text;
            }

            Column {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                Text {
                    visible: geofenceCB.checked || analysisCB.checked
                    text: qsTr("the target")
                    color: Material.foreground
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                ButtonGroup {
                    id: singleOrAllObjects
                }

                RadioButton {
                    id: allObjectRb
                    visible: geofenceCB.checked || analysisCB.checked
                    text: qsTr("objects from")
                    ButtonGroup.group: singleOrAllObjects
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                }

                Row {
                    visible: geofenceCB.checked || analysisCB.checked
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
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        font.bold: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter

                        placeholderText: "<object ID>"
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
                        iconUrl: DsaResources.iconGps
                        opacity: enabled ? 1.0 : 0.8
                        anchors.verticalCenter: featureIdEdit.verticalCenter
                        onClicked: {
                            toolController.togglePickMode();
                        }
                    }
                }

                ComboBox {
                    visible: geofenceCB.checked || analysisCB.checked
                    id: targetCB
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    width: 256 * scaleFactor
                    textRole: "display"
                    model: controller.targetNames
                    currentIndex: -1
                }
            }

            TextField {
                id: attributeValueEdit
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    margins: 16 * scaleFactor
                }
                visible: attributeCB.checked
                color: Material.accent
                width: attributeFieldEdit.width
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                placeholderText: "<enter value>"
            }
        }

        // Review
        Item {
            id: reviewPage

            Text {
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
