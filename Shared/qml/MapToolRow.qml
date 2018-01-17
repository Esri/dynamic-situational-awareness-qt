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
import Esri.DSA 1.0
import QtQuick.Controls.Material 2.2

Item {
    Row {
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
        }
        spacing: 5 * scaleFactor

        ToolIcon {
            id: homeIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png"
            toolName: "Basemap"
            onToolSelected: selected = !selected
        }
    }
}
