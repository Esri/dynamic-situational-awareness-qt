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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.3

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


