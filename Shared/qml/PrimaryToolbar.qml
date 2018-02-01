// Copyright 2016 ESRI
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0
import QtQuick.Controls.Material 2.2

ToolBar {
    property alias toolbarLabelText: toolbarLabel.text
    property int marginWidth: 5 * scaleFactor
    property int imageWidth: 36 * scaleFactor
    property bool menuVisible

    Row {
        anchors {
            fill: parent
            margins: 10 * scaleFactor
        }

        spacing: 5 * scaleFactor

        Image {
            anchors.verticalCenter: parent.verticalCenter
            width: DsaStyles.secondaryIconSize * scaleFactor
            height: width
            source: DsaResources.iconDrawer
            visible: menuVisible

            MouseArea {
                anchors.fill: parent
                onClicked: categoryToolbar.visible = !categoryToolbar.visible
            }
        }

        Label {
            id: toolbarLabel
            anchors.verticalCenter: parent.verticalCenter

            font {
                pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
            }
            color: Material.foreground
        }

    }
}

