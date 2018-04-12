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
    property string instruction: "Location of observation"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property alias controlPoint: controlPointTextField.text
    property alias locationDescription: locationField.text

    function clear() {
        locationField.text = "";

        if (visible)
            locationField.forceActiveFocus();
    }

    function text() {
        return "location:" + controlPointTextField.text;
    }

    onVisibleChanged: {
        if (visible)
            locationField.forceActiveFocus();
    }

    TextField {
        id: controlPointTextField
        anchors {
            left: locationField.left
            right: locationField.right
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

        placeholderText: "<lat long of observation>"
    }


    Row {
        id: pickRow
        anchors {
            top: controlPointTextField.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 8 * scaleFactor
        }

        width: parent.width * 0.5

        spacing: 16 * scaleFactor

        ToolIcon {
            id: pickButton

            selected: toolController.pickMode
            toolName: "From Map"
            iconSource: DsaResources.iconTouch
            opacity: enabled ? 1.0 : 0.8

            onToolSelected: {
                toolController.togglePickMode();
            }
        }

        ToolIcon {
            id: myLocationButton

            toolName: "My Location"
            iconSource: DsaResources.iconGps
            opacity: enabled ? 1.0 : 0.8

            onToolSelected: {
                toolController.setFromMyLocation();
            }
        }
    }

    TextEdit {
        id: locationField
        clip: true
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: pickRow.bottom
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
            visible: locationField.text.length === 0
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
            top : locationField.bottom
            right: locationField.right
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
