
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
    id: createAlertsRoot
    width: 272 * scaleFactor
    title: qsTr("Edit Alert Rules")
    clip: true

    EditAlertsController {
        id: toolController
    }

    property bool readyToAdd: statusCb.currentIndex !== -1 &&
                              leftHandSideCB.currentIndex !== -1 &&
                              layerCB.currentIndex !== -1 &&
                              featureIdEdit.text.length > 0
    property bool expandNewAlert: true

    Rectangle {
        id: newAlertFrame
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }
        height: expandNewAlert ? parent.height * 0.6 : addButton.height + (anchors.margins * 2)
        color: "transparent"
        border.color: Material.foreground
        border.width: 1 * scaleFactor
        radius: 12
    }

    Image {
        id: collapseNewAlertButton
        anchors {
            margins: 8 * scaleFactor
            top: newAlertFrame.top
            left: newAlertFrame.left
        }

        width: 42 * scaleFactor
        height: 48 * scaleFactor
        source: "qrc:/Resources/icons/xhdpi/drawer_large.png"

        MouseArea {
            anchors.fill: parent
            onClicked: expandNewAlert = !expandNewAlert;
        }
    }

    Text {
        id: newAlertText
        anchors {
            margins: 8 * scaleFactor
            verticalCenter: collapseNewAlertButton.verticalCenter
            horizontalCenter: newAlertFrame.horizontalCenter
        }
        text: qsTr("New alert")
        color: Material.foreground
        font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }

    Column {
        clip: true
        spacing: 16 * scaleFactor

        anchors {
            top: newAlertText.bottom
            left: newAlertFrame.left
            right: newAlertFrame.right
            bottom: newAlertFrame.bottom
            margins: 8 * scaleFactor
        }

        ComboBox {
            id: statusCb
            anchors {
                left: parent.left
                right: parent.right
            }

            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            textRole: "display"
            model: toolController.statusNames
            currentIndex: -1
        }

        Text {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: qsTr("Alert when")
            color: Material.foreground
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            horizontalAlignment: Text.AlignLeft
        }

        ComboBox {
            id: leftHandSideCB
            anchors {
                left: parent.left
                right: parent.right
            }

            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            textRole: "display"
            model: toolController.layerNames
            currentIndex: -1
        }

        ButtonGroup {
            id: spatialOperationGroup
        }

        RadioButton {
            anchors {
                left: parent.left
                right: parent.right
            }
            id: intersectsRb
            text: "intersects"
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            checked: true
            ButtonGroup.group: spatialOperationGroup
        }

        Row {
            spacing: 8 * scaleFactor
            RadioButton {
                id: withinDistanceRb
                text: "is within"
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                ButtonGroup.group: spatialOperationGroup
            }

            SpinBox {
                id: withinDistanceSB
                anchors.verticalCenter: withinDistanceRb.verticalCenter
                enabled: withinDistanceRb.checked
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                value: 50
            }

            Text {
                anchors.verticalCenter: withinDistanceRb.verticalCenter
                enabled: withinDistanceRb.checked
                text: qsTr("m of")
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        Row {
            spacing: 8 * scaleFactor
            height: pickButton.height
            Text {
                id: featureIdLabel
                text: qsTr("feature")
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: featureIdEdit
                validator: IntValidator{}
                color: Material.accent
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Button {
                id: pickButton
                anchors.verticalCenter: featureIdLabel.verticalCenter
                height: 28 * scaleFactor
                width:  height

                Image {
                    anchors.centerIn: parent
                    width: pickButton.height
                    height: width
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_gpson_dark_d.png"
                }

                onClicked: {
                }
            }
        }

        ComboBox {
            id: layerCB
            anchors {
                left: parent.left
                right: parent.right
            }
            font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor

            textRole: "display"
            model: toolController.layerNames
            currentIndex: -1
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
                if (withinDistanceRb.checked)
                {
                    toolController.addWithinDistanceAlert(
                                statusCb.currentIndex,
                                leftHandSideCB.currentIndex,
                                withinDistanceSB.value,
                                Number(featureIdEdit.text),
                                layerCB.currentIndex);
                }
            }
        }
    }

    ListView {
        id: alertsList
        clip: true
        anchors {
            margins: 8 * scaleFactor
            top: newAlertFrame.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: alertsModel
        delegate: Row {
            Text {
                text: "Fruit: " + name
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            }
            Text {
                text: "Cost: $" + cost
                color: Material.foreground
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            }
        }
    }

    ListModel {
        id: alertsModel
        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
    }
}
