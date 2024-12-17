/*******************************************************************************
 *  Copyright 2012-2024 Esri
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
import QtQuick.Layouts
import Esri.ArcGISRuntime.OpenSourceApps.DSA

DsaPanel {
    id: configurationsRoot
    title: qsTr("Configurations")

    ConfigurationController {
        id: toolController
    }

    Component {
        id: configurationListItemDelegate

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
            }
            color: Material.backgroundColor
            height: 40 * scaleFactor
            width: parent.width

            CheckBox {
                id: checkboxSelected
                anchors {
                    left: parent.left
                }
                checked: model.Selected
                enabled: model.Downloaded
                onClicked: {
                    toolController.select(index)
                    checked = Qt.binding(function () { // restore the binding
                        return model.Selected;
                    });
                }
            }

            Label {
                id: labelName
                anchors {
                    left: checkboxSelected.right
                }
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: model.Name
            }

            Image {
                id: imageRequiresReload
                source: DsaResources.iconAlertModerate
                anchors {
                    left: labelName.right
                }
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                width: model.Selected && !model.Loaded ? parent.height : 0
            }

            ProgressBar {
                id: progressBarPercentDownloaded
                anchors {
                    left: imageRequiresReload.right
                    right: imageCancel.left
                    verticalCenter: parent.verticalCenter
                }
                visible: model.PercentDownloaded > 0 && model.PercentDownloaded < 100
                to: 100
                value: model.PercentDownloaded
            }

            Image {
                id: imageCancel
                source: DsaResources.iconAlertModerate
                height: parent.height
                width: model.PercentDownloaded > 0 && model.PercentDownloaded < 100 ? parent.height : 0
                anchors {
                    right: imageDownload.left
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: toolController.cancel(index)
                }
            }

            Image {
                id: imageDownload
                source: "qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png"
                height: parent.height
                width: !model.Downloaded ? parent.height : 0
                enabled: !toolController.downloading
                anchors {
                    right: imageRemove.left
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: toolController.download(index)
                }
            }

            Image {
                id: imageRemove
                source: DsaResources.iconTrash
                height: parent.height
                width: model.Downloaded ? parent.height : 0
                anchors {
                    right: parent.right
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: toolController.remove(index)
                }
            }
        }
    }

    ListView {
        id: configurationList
        anchors {
            top: configurationsRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        interactive: true
        clip: true
        spacing: 2 * scaleFactor
        model: toolController.configurations
        delegate: configurationListItemDelegate
    }
}
