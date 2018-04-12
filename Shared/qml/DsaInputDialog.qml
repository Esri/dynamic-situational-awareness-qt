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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0
import Esri.DSA 1.0

Dialog {
    property alias inputLabel: inputLabelText.text
    property alias inputPlaceholderText: nameText.placeholderText
    property alias userInputText: nameText.text
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property alias titleText: titleLabel.text
    visible: false
    x: appRoot.width / 2 - width / 2
    y: appRoot.height / 2 - height / 2
    width: 250 * scaleFactor
    height: 120 * scaleFactor
    standardButtons: Dialog.Ok | Dialog.Cancel

    Column {
        width: parent.width
        spacing: 10 * scaleFactor

        Label {
            id: titleLabel
            anchors.horizontalCenter: parent.horizontalCenter
            font {
                family: DsaStyles.fontFamily
                pixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
                bold: true
            }
        }

        Row {
            width: parent.width
            spacing: 5 * scaleFactor

            Label {
                id: inputLabelText
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.4
                font {
                    family: DsaStyles.fontFamily
                    pixelSize: 12 * scaleFactor
                }
            }

            TextField {
                id: nameText
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.6
            }
        }
    }

    onOpened: {
        nameText.text = "";
        nameText.focus = true;
    }
}
