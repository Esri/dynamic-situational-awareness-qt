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
import Esri.DSA 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Row {
    id: reportToolRow
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "home"
    onVisibleChanged: state = "clear"

    Image {
        id: optionsIcon
        source: DsaResources.iconSettings
        width: DsaStyles.primaryIconSize
        height: width

        MouseArea {
            anchors.fill: parent
            onClicked: menu.open()
        }

        Menu {
            id: menu
            MenuItem {
                text: "About"
                onTriggered: aboutTool.visible = true
            }
            MenuItem {
                text: "Settings"
                onTriggered: optionsTool.visible = true
            }
            MenuItem {
                text: "Close"
                onTriggered: appRoot.showCloseDialog("Are you sure you want to close?");
            }
        }
    }
}


