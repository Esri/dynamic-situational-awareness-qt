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
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.1

DsaPanel {
    id: localDataRoot
    property bool showDataConnectionPane: true
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property var selectedItems: []
    title: "Add Local Data"
    width: 272 * scaleFactor

    // Create the controller
    AddLocalDataController {
        id: toolController
    }

    Connections {
        target: appRoot
        function onMarkupLayerReceived(path, overlayVisible) {
            toolController.createMarkupLayer(path, -1, overlayVisible);
        }
    }

    // Declare the ListView, which will display the list of files
    ListView {
        id: localDataList

        property string currentPath: ""

        anchors {
            top: localDataRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: filterColumn.top
            margins: 8 * scaleFactor
        }

        clip: true
        model: toolController.localDataModel
        width: parent.width
        delegate:  CheckDelegate {
            id: control
            text: fileName
            width: localDataList.width

            contentItem: Label {
                rightPadding: control.indicator.width + control.spacing
                text: control.text
                font: control.font
                opacity: enabled ? 1.0 : 0.3
                color: Material.foreground
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                if (checkState === Qt.Checked) {
                    // add the selected item index to the array
                    selectedItems.push(index);
                } else if (checkState === Qt.Unchecked) {
                    // remove the selected item index in the array
                    selectedItems.splice(selectedItems.indexOf(index), 1);
                }
            }
        }
    }

    // filter the files by file type
    Column {
        id: filterColumn
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 20 * scaleFactor
        }
        spacing: 1 * scaleFactor

        Label {
            text: "Filter:"
            font.pixelSize: 12 * scaleFactor
            color: Material.foreground
        }

        ComboBox {
            id: filter
            model: toolController.fileFilterList
            width: parent.width
            onCurrentTextChanged: {
                selectedItems = [];
                toolController.refreshLocalDataModel(currentText);
            }
        }

        CheckBox {
            id: elevationCheckbox
            text: "Add as elevation source"
            checked: false
            visible: filter.currentText.indexOf("Raster") !== -1 || filter.currentText.indexOf("All")  !== -1 || filter.currentText.indexOf("tpk") !== -1
            contentItem: Label {
                text: elevationCheckbox.text
                font: elevationCheckbox.font
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: Material.foreground
                leftPadding: elevationCheckbox.indicator.width + elevationCheckbox.spacing
            }
        }

        // When this button is clicked, all checked items will be added as layers/elevation sources
        Button {
            id: addButton
            opacity: 0.95
            text: "Add Selected"
            width: parent.width

            background: Rectangle {
                implicitWidth: filterColumn.width
                implicitHeight: 40 * scaleFactor
                opacity: enabled ? 1 : 0.3
                color: Material.accent
                radius: 2 * scaleFactor
            }

            onClicked: {
                if (elevationCheckbox.checked && elevationCheckbox.visible)
                    toolController.addItemAsElevationSource(selectedItems);
                else
                    toolController.addItemAsLayer(selectedItems);
                selectedItems = []; // clear so we don't add again next time
                toolController.refreshLocalDataModel(filter.currentText);
                closed();
            }
        }
    }

    function resetView() {
        selectedItems = [];
        toolController.refreshLocalDataModel(filter.currentText);
    }
}
