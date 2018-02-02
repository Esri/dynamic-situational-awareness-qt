
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
    width: 272 * scaleFactor
    title: qsTr("Alert Conditions")
    clip: true

    AlertConditionsController {
        id: toolController
        active: parent.visible
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

    property bool readyToAdd: geofenceReadyToAdd || attributeReadyToAdd || analysisReadyToAdd

    property bool geofenceReadyToAdd:  geofenceConditionButton.checked && (levelCb.currentIndex !== -1 &&
                                                                           sourceCb.currentIndex !== -1 &&
                                                                           targetCB.currentIndex !== -1 &&
                                                                           (featureIdEdit.text.length > 0 || allObjectRb.checked))
    property bool attributeReadyToAdd: attributeConditionButton.checked && (levelCb.currentIndex !== -1 &&
                                                                            sourceCb.currentIndex !== -1 &&
                                                                            attributeFieldEdit.length > 0 &&
                                                                            attributeValueEdit.length > 0)
    property bool analysisReadyToAdd: analysisConditionButton.checked && (levelCb.currentIndex !== -1 &&
                                                                          sourceCb.currentIndex !== -1 &&
                                                                          targetCB.currentIndex !== -1 &&
                                                                          (featureIdEdit.text.length > 0 || allObjectRb.checked))
    Row {
        id: newOrViewRow
        spacing: 0

        anchors {
            margins: 8 * scaleFactor
            left: parent.left
            right: parent.right
            top: titleBar.bottom
        }

        ButtonGroup {
            id: newOrViewGroup
        }

        Button {
            id: createNewModeButton
            text: "Create new"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            font.bold: checked
            width: (newOrViewRow.width - (newOrViewRow.anchors.margins * 2)) * 0.5
            checked: true
            checkable: true
            ButtonGroup.group: newOrViewGroup
            background: Rectangle {
                color: createNewModeButton.checked ? Material.accent : Material.primary
                border.color: Material.foreground
            }
        }

        Button {
            id: viewExistingModeButton
            text: "View existing"
            font.pixelSize: createNewModeButton.font.pixelSize
            font.bold: checked
            width: createNewModeButton.width
            checked: false
            checkable: true
            ButtonGroup.group: newOrViewGroup
            background: Rectangle {
                color: viewExistingModeButton.checked ? Material.accent : Material.primary
                border.color: Material.foreground
            }
        }
    }

    Column {
        clip: true
        spacing: 4 * scaleFactor

        anchors {
            top: newOrViewRow.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }
        visible: createNewModeButton.checked

        ComboBox {
            id: levelCb
            anchors {
                left: parent.left
                right: parent.right
            }

            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            textRole: "display"
            model: toolController.levelNames
            currentIndex: -1

            Text {
                anchors.centerIn: parent
                visible: parent.currentIndex === -1
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                color: Material.accent
                text: "<select priority>"
            }
        }

        Row {
            id: conditionTypeRow
            spacing: 0

            ButtonGroup {
                id: conditionTypeGroup
            }

            Button {
                id: geofenceConditionButton
                text: "GeoFence\nAlert"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: checked
                height: 32 * scaleFactor
                width: (newOrViewRow.width - (newOrViewRow.anchors.margins * 2)) / 3
                checked: true
                checkable: true
                ButtonGroup.group: conditionTypeGroup
                background: Rectangle {
                    color: geofenceConditionButton.checked ? Material.accent : Material.primary
                    border.color: Material.foreground
                }
            }

            Button {
                id: attributeConditionButton
                text: "Attribute\nAlert"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: checked
                height: geofenceConditionButton.height
                width: geofenceConditionButton.width
                checked: false
                checkable: true
                ButtonGroup.group: conditionTypeGroup
                background: Rectangle {
                    color: attributeConditionButton.checked ? Material.accent : Material.primary
                    border.color: Material.foreground
                }
            }

            Button {
                id: analysisConditionButton
                text: "Analysis\nAlert"
                enabled: false
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: checked
                height: geofenceConditionButton.height
                width: geofenceConditionButton.width
                checked: false
                checkable: true
                ButtonGroup.group: conditionTypeGroup
                background: Rectangle {
                    color: analysisConditionButton.checked ? Material.accent : Material.primary
                    border.color: Material.foreground
                }
            }
        }

        Row {
            spacing: 8 * scaleFactor
            height: newAlertName.height

            Text {
                text: qsTr("called")
                anchors.verticalCenter: newAlertName.verticalCenter
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: newAlertName
                color: Material.accent
                font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
                font.bold: true
                width: 235 * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                placeholderText: "<enter name>"
            }
        }

        Text {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: qsTr("when objects from")
            color: Material.foreground
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            horizontalAlignment: Text.AlignLeft
        }

        ComboBox {
            id: sourceCb
            anchors {
                left: parent.left
                right: parent.right
            }

            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            textRole: "display"
            model: toolController.sourceNames
            currentIndex: -1

            Text {
                anchors.fill: parent
                visible: parent.currentIndex === -1
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                color: Material.accent
                text: "<source feed>"
            }
        }

        ButtonGroup {
            id: spatialOperationGroup
        }

        RadioButton {
            id: withinAreaRb
            visible: geofenceConditionButton.checked
            anchors {
                left: parent.left
                right: parent.right
            }
            text: "are within area of"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            checked: true
            ButtonGroup.group: spatialOperationGroup
        }

        Row {
            visible: geofenceConditionButton.checked
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
            visible: attributeConditionButton.checked
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

            TextField {
                id: attributeValueEdit
                color: Material.accent
                width: attributeFieldEdit.width
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                placeholderText: "<enter value>"
            }
        }

        Text {
            id: visibleLabel
            visible: analysisConditionButton.checked
            text: qsTr("can be seen by")
            color: Material.foreground
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            visible: geofenceConditionButton.checked || analysisConditionButton.checked
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
            visible: geofenceConditionButton.checked || analysisConditionButton.checked
            text: qsTr("objects from")
            ButtonGroup.group: singleOrAllObjects
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
        }

        Row {
            visible: geofenceConditionButton.checked || analysisConditionButton.checked
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
            visible: geofenceConditionButton.checked || analysisConditionButton.checked
            id: targetCB
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

            textRole: "display"
            model: toolController.targetNames
            currentIndex: -1

            Text {
                anchors.fill: targetCB
                visible: parent.currentIndex === -1
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "<target overlay>"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                color: Material.accent
            }
        }

        RoundButton {
            id: addButton
            enabled: readyToAdd

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            background: Rectangle {
                implicitWidth: 40 * scaleFactor
                implicitHeight: 40 * scaleFactor
                opacity: enabled ? 1 : 0.3
                radius: addButton.radius
                color: Material.accent

                Image {
                    anchors.centerIn: parent
                    width: 26 * scaleFactor
                    height: width
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"
                }
            }

            onClicked: {
                if (geofenceConditionButton.checked) {
                    if (withinDistanceRb.checked) {
                        toolController.addWithinDistanceAlert(newAlertName.text,
                                                              levelCb.currentIndex,
                                                              sourceCb.currentText,
                                                              withinDistanceSB.value,
                                                              singleFeatureRb.checked ? Number(featureIdEdit.text) : -1,
                                                                                        targetCB.currentIndex);
                    } else if (withinAreaRb.checked) {
                        toolController.addWithinAreaAlert(newAlertName.text,
                                                          levelCb.currentIndex,
                                                          sourceCb.currentText,
                                                          singleFeatureRb.checked ? Number(featureIdEdit.text) : -1,
                                                                                    targetCB.currentIndex);
                    }
                } else if (attributeConditionButton.checked) {
                    toolController.addAttributeEqualsAlert(newAlertName.text,
                                                           levelCb.currentIndex,
                                                           sourceCb.currentText,
                                                           attributeFieldEdit.text,
                                                           attributeValueEdit.text);
                }

                levelCb.currentIndex = -1;
                sourceCb.currentIndex = -1;
                featureIdEdit.text = "";
                targetCB.currentIndex = "";
                attributeFieldEdit.text = "";
                attributeValueEdit.text = "";
                newAlertName.text = "";
            }
        }
    }

    Text {
        id: conditionsListTitle
        visible: viewExistingModeButton.checked
        anchors {
            margins: 8 * scaleFactor
            top: newOrViewRow.bottom
            horizontalCenter: parent.horizontalCenter
        }
        text: qsTr("Conditions")
        color: Material.foreground
        font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }

    ListView {
        id: conditionsList
        visible: viewExistingModeButton.checked
        clip: true
        anchors {
            margins: 8 * scaleFactor
            top: conditionsListTitle.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: toolController.conditionsList
        currentIndex: -1

        delegate: Rectangle {
            width: conditionsList.width
            height: expandRowButton.height + (8 * scaleFactor)
            color: Material.background
            radius: 2 * scaleFactor

            Text {
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    right: expandRowButton.left
                    margins: 8 * scaleFactor
                }
                text: name
                color: index === conditionsList.currentIndex ? Material.accent : Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WrapAnywhere
                elide: Text.ElideRight
            }

            OverlayButton {
                id: expandRowButton
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 8 * scaleFactor
                }
                iconUrl: DsaResources.iconDrawer

                onClicked: {
                    conditionsMenu.visible = true;
                }
            }

            Rectangle {
                id: conditionsMenu
                visible: false
                color: Material.background
                border.color: Material.accent
                border.width: 2 * scaleFactor
                radius: 8 * scaleFactor
                width: 64 * scaleFactor
                height: 128 * scaleFactor

                anchors {
                    right: expandRowButton.right
                    top: expandRowButton.top
                }

                Column {
                    id: conditionsCol
                    anchors{
                        margins: 8 * scaleFactor
                        fill: parent
                    }
                    spacing: 4 * scaleFactor

                    Button {
                        width: parent.width
                        text: "Edit"
                        enabled: false
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        onClicked: toolController.removeConditionAt(index);
                    }

                    Button {
                        width: parent.width
                        text: "Delete"
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        onClicked: toolController.removeConditionAt(index);
                    }

                    Button {
                        width: parent.width
                        text: "Pause"
                        enabled: false
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        onClicked: toolController.removeConditionAt(index);
                    }

                    Button {
                        width: parent.width
                        text: "Close"
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        onClicked: conditionsMenu.visible = false;
                    }

                }
            }
        }
    }

    Text {
        anchors {
            left: parent.left
            right: parent.right
            top: conditionsListTitle.bottom
            margins: 15 * scaleFactor
        }
        visible: conditionsList.count === 0 && viewExistingModeButton.checked
        text: "No Conditions.\n\nSelect 'Create New' to set up alert queries."
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
    }
}
