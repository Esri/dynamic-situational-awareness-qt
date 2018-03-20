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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    property alias itemChecked: visibleCheckBox.checked
    property url imageUrl
    property bool imageVisible
    property bool imageFrameVisible: true
    property alias checkBoxVisible: visibleCheckBox.visible
    property string mainText
    property bool menuIconVisible: false
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property color imageFrameColor: Material.foreground

    Row {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: 5 * scaleFactor
        }

        id: itemRow
        width: parent.width
        spacing: 3 * scaleFactor

        CheckBox {
            id: visibleCheckBox
            anchors.verticalCenter: parent.verticalCenter
            checked: itemChecked
            onClicked: itemChecked = checked;
        }

        Rectangle {
            id: iconImage
            anchors.verticalCenter: parent.verticalCenter
            radius: 50 * scaleFactor
            width: 24 * scaleFactor
            height: width
            color: imageFrameVisible ? imageFrameColor : "transparent"
            visible: imageVisible

            Image {
                anchors {
                    fill: parent
                    margins: 2 * scaleFactor
                }
                source: imageUrl
            }
        }

        Label {
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideMiddle
            text: mainText
            verticalAlignment: Text.AlignVCenter
            width: menuIconVisible ? parent.width * 0.6 : parent.width
            color: Material.foreground
            font {
                pixelSize: 14 * scaleFactor
                bold: true
                family: DsaStyles.fontFamily
            }
        }
    }

    MouseArea {
        anchors.fill: itemRow
        onClicked: visibleCheckBox.checked = !visibleCheckBox.checked
    }
}
