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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.0

Item {
    id: reportDatePage

    property bool valid: controlPointTextField.length > 0
    property string instruction: "Location of observation"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

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

        spacing: 20 * scaleFactor

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
