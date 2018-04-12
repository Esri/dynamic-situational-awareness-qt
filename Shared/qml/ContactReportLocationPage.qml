
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
    id: reportDatePage

    property bool valid: controlPointTextField.length > 0
    property string instruction: "Location"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property alias controlPoint: controlPointTextField.text
    property alias locationDescription: enemyLocationField.text

    function clear() {
        enemyLocationField.text = "";

        if (visible)
            enemyLocationField.forceActiveFocus();
    }

    function text() {
        return "location:" + controlPointTextField.text;
    }

    onVisibleChanged: {
        if (visible)
            enemyLocationField.forceActiveFocus();
    }

    TextField {
        id: controlPointTextField
        anchors {
            left: enemyLocationField.left
            right: enemyLocationField.right
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        color: Material.accent
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: toolController.controlPoint;

        placeholderText: "<lat long of enemy>"
    }

    OverlayButton {
        id: pickButton

        anchors {
            top: controlPointTextField.bottom
            right: parent.horizontalCenter
        }

        selected: toolController.pickMode
        iconUrl: DsaResources.iconTouch
        opacity: enabled ? 1.0 : 0.8

        onClicked: {
            toolController.togglePickMode();
        }
    }

    OverlayButton {
        id: myLocationButton

        anchors {
            top: controlPointTextField.bottom
            left: parent.horizontalCenter
        }

        iconUrl: DsaResources.iconGps

        onClicked: {
            toolController.setFromMyLocation();
        }
    }

    TextEdit {
        id: enemyLocationField
        clip: true
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: pickButton.bottom
            margins: 16 * scaleFactor
        }
        width: parent.width * 0.75
        height: parent.height * 0.50
        color: Material.foreground
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        padding: 4 * scaleFactor
        wrapMode: Text.Wrap
        selectByKeyboard: true
        selectByMouse: true

        onTextChanged: {
            if (length > 250)
                remove(250, length -1);
        }

        Rectangle {
            anchors.fill: parent
            border {
                color: Material.accent
                width: 1 * scaleFactor
            }
            color: "transparent"
        }

        Text {
            anchors.centerIn: parent
            visible: enemyLocationField.text.length === 0
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
                italic: true
            }
            opacity: 0.5
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Material.accent

            text: "<enter location description>"
        }
    }

    Button {
        anchors {
            top : enemyLocationField.bottom
            right: enemyLocationField.right
            margins: 4 * scaleFactor
        }
        text: "clear"

        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            family: DsaStyles.fontFamily
        }

        onClicked: clear();
    }
}
