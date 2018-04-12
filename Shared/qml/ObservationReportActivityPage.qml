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

    property bool valid: activity.length > 0
    property string instruction: "Activity observed"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property alias activity: activityField.text

    function clear() {
        activity = "";

        if (visible)
            activityField.forceActiveFocus();
    }

    function text() {
        return "activity:" + activityField.text;
    }

    onVisibleChanged: {
        if (visible)
            activityField.forceActiveFocus();
    }

    TextEdit {
        id: activityField
        clip: true
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
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
            visible: activityField.text.length === 0
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
                italic: true
            }
            opacity: 0.5
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Material.accent

            text: "<enter activity>"
        }
    }

    Button {
        anchors {
            top : activityField.bottom
            right: activityField.right
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
