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
            model: [
                "ArcGIS Online/Portal item",
                "Web resource",
                "File resource"
            ]
        }

        Text {
            color: Material.foreground
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 5
            Layout.topMargin: 5
            Layout.rightMargin: 5
            text: "Name for the configuration"
        }

        TextField {
            id: txtConfigurationName
            Layout.fillWidth: true
            Layout.leftMargin: 5
            Layout.topMargin: 5
            Layout.rightMargin: 5
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

            Image {
                source: DsaResources.iconSave
                visible: comboResourceType.currentIndex === 2
                Layout.alignment: Qt.AlignRight
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
        }

        Rectangle {
            Layout.fillHeight: true
        }
    }
}
