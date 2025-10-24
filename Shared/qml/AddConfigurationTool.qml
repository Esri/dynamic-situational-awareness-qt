/*******************************************************************************
 *  Copyright 2012-2025 Esri
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
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

DsaPanel {
    id: addConfigurationRoot
    width: 272 * scaleFactor
    title: qsTr("Add Configuration")

    readonly property RegularExpressionValidator configRegexName: RegularExpressionValidator { regularExpression: /^[A-Za-z0-9_]+$/ }
    readonly property RegularExpressionValidator configRegexItem: RegularExpressionValidator { regularExpression: /^https:\/\/.+\/item\.html\?id=[A-Fa-f0-9]{32}?$/ }
    readonly property RegularExpressionValidator configRegexWeb: RegularExpressionValidator { regularExpression: /^https:\/\/.+/ }
    readonly property RegularExpressionValidator configRegexFile: RegularExpressionValidator { regularExpression: /^file:\/\/.+/ }

    property var configTypeNames: [
        "ArcGIS Online/Portal Item",
        "Web Resource",
        "File Resource"
    ]
    property var configHints: [
        "https://.../item.html?id=82ce2d85e21c4326bc072d441b636e5e",
        "https://server.com/folder/file.zip",
        "file://server/folder/file.zip"
    ]
    property var configRegex: [
        configRegexItem,
        configRegexWeb,
        configRegexFile
    ]

    ColumnLayout {
        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom
        width: parent.width

        Label {
            color: Material.foreground
            Layout.alignment: Qt.AlignLeft
            text: "Resource Type"
            Layout.leftMargin: 5
            Layout.topMargin: 5
            Layout.rightMargin: 5
        }

        ComboBox {
            id: comboResourceType
            Layout.fillWidth: true
            Layout.leftMargin: 5
            Layout.topMargin: 5
            Layout.rightMargin: 5
            model: configTypeNames
            onCurrentValueChanged: {
                txtUrl.validator = configRegex[configTypeNames.indexOf(comboResourceType.currentValue)]
                txtUrl.placeholderText = configHints[configTypeNames.indexOf(comboResourceType.currentValue)]
            }
        }

        RowLayout {
            Text {
                Layout.leftMargin: 5
                Layout.topMargin: 5
                Layout.rightMargin: 5
                Layout.fillWidth: true
                color: Material.foreground
                Layout.alignment: Qt.AlignLeft
                text: "Name for the configuration"
            }
            Text {
                Layout.leftMargin: 5
                Layout.topMargin: 5
                Layout.rightMargin: 5
                id: txtNameAlreadyInUse
                color: "red"
                Layout.alignment: Qt.AlignRight
                font {
                    italic: true
                }
                visible: configurationController.nameAlreadyInUse(txtConfigurationName.text)
                text: "already in use"
            }
        }

        TextField {
            id: txtConfigurationName
            Layout.fillWidth: true
            Layout.leftMargin: 5
            Layout.topMargin: 5
            Layout.rightMargin: 5
            validator: configRegexName
        }

        Label {
            color: Material.foreground
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 5
            Layout.topMargin: 5
            Layout.rightMargin: 5
            text: "URL"
        }

        RowLayout {

            TextField {
                id: txtUrl
                Layout.fillWidth: true
                Layout.leftMargin: 5
                Layout.topMargin: 5
                Layout.rightMargin: 5
            }

            CalciteIcon {
                iconName: CalciteIcons.Calcite_folder_open
                visible: comboResourceType.currentIndex === 2
                height: 24 * scaleFactor
                width: height
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        fileDialog.open();
                    }
                }
            }


            FileDialog {
                id: fileDialog
                nameFilters: ["Zip files (*.zip)"]
                onAccepted: {
                    txtUrl.text = selectedFile
                }
            }
        }

        Button {
            text: "Add"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                configurationController.addConfiguration(txtUrl.text, txtConfigurationName.text);
                comboResourceType.currentIndex = 0;
                txtUrl.clear();
                txtConfigurationName.clear();
                drawer.close();
            }
            enabled: txtUrl.length > 0 && txtConfigurationName.length > 0 && !txtNameAlreadyInUse.visible
        }

        Rectangle {
            Layout.fillHeight: true
        }
    }
}
