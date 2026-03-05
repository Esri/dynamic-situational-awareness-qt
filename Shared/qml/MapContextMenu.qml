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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Menu {
    id: contextMenu
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    visible: contextMenuController.contextActive
    x: contextMenuController.contextScreenPosition.x
    y: contextMenuController.contextScreenPosition.y
    width: 110 * scaleFactor

    ContextMenuController {
        id: contextMenuController

        onResultChanged: {
            msgDialog.title = contextMenuController.resultTitle;
            msgDialog.informativeText = contextMenuController.result;
            msgDialog.open();
        }
    }

    onVisibleChanged: {
        if (!visible)
            contextMenuController.contextActive = visible;
    }

    Repeater {
        id: optionsRepeater
        model: contextMenuController.options
        delegate: ListLabel {
            text: display
            separatorVisible: index !== contextMenuController.options.rowCount() - 1
            onTriggered: {
                contextMenuController.selectOption(display);
            }
        }
    }
}
