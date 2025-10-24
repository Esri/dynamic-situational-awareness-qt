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
import QtQuick.Layouts
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: optionsRoot
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    Connections {
        target: configurationController
        function onConfigurationDownloadFailed(configurationName, message) {
            configurationDialogConfirmRemove.configurationName = configurationName;
            configurationDialogConfirmRemove.alsoRemoveEntry = true;
            configurationDialogConfirmRemove.confirmationMessage = "'" + configurationName + "' " + message + "\nRemove it from the list?";
            configurationDialogConfirmRemove.open();
        }
    }

    color: Material.primary

    function showConfigurationsTab() {
        visible = true;
        bar.setCurrentIndex(1);
    }

    // Add the Options Controller Class
    OptionsController {
        id: optionsController
    }

    TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Options")
        }
        TabButton {
            text: qsTr("Configurations")
        }
    }

    StackLayout {
        id: stackLayout
        width: parent.width
        anchors {
            top: bar.bottom
            bottom: buttonDismiss.top
        }

        currentIndex: bar.currentIndex

        // Create a flickable column so that n number of options can be added
        Item {
            Flickable {
                id: optionsFlickable

                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }
                contentHeight: optionsColumn.height + parent.height * .15

                clip: true
                Column {
                    id: optionsColumn
                    width: parent.width

                    spacing: 10 * scaleFactor

                    Label {
                        text: "Map"
                        font {
                            family: DsaStyles.fontFamily
                            underline: true
                            pixelSize: DsaStyles.titleFontPixelSize * 0.75
                        }
                        color: Material.foreground
                    }

                    // Toggle navigation controls
                    CheckBox {
                        text: "Show navigation controls"
                        checked: true
                        onCheckedChanged: {
                            // update visibility of UI components
                            navTool.visible = checked;
                            compass.visible = checked;
                        }
                    }

                    // Toggle location/elevation overlay
                    CheckBox {
                        text: "Show location and elevation"
                        checked: true
                        onCheckedChanged: {
                            // update visibility of UI component
                            currentLocation.visible = checked;
                        }
                    }

                    // Toggle friendly tracks labels
                    CheckBox {
                        text: "Show friendly tracks labels"
                        checked: true
                        onCheckedChanged: {
                            optionsController.showFriendlyTracksLabels = checked;
                        }
                    }

                    Label {
                        text: "Location"
                        font {
                            family: DsaStyles.fontFamily
                            underline: true
                            pixelSize: DsaStyles.titleFontPixelSize * 0.75
                        }
                        color: Material.foreground
                    }

                    // Whether to use GPS for the location/elevation display or not.
                    // The alternative is the use the Scene's base surface.
                    CheckBox {
                        id: useGPS
                        text: "Use GPS for current elevation display"
                        checked: optionsController.useGpsForElevation
                        onCheckedChanged: optionsController.useGpsForElevation = checked
                    }


                    CheckBox {
                        text: "Location Broadcast"
                        checked: messageFeeds.controller.locationBroadcastEnabled
                        onCheckedChanged: messageFeeds.controller.locationBroadcastEnabled = checked
                    }

                    Row {
                        height: 40 * scaleFactor
                        spacing: 5 * scaleFactor

                        Text {
                            text: "Location Broadcast frequency (ms)"
                            color: Material.foreground
                            font {
                                pixelSize: 10 * scaleFactor
                                family: DsaStyles.fontFamily
                            }
                        }

                        TextField {
                            width: 50 * scaleFactor
                            text: messageFeeds.controller.locationBroadcastFrequency
                            color: Material.foreground
                            font {
                                pixelSize: 10 * scaleFactor
                                family: DsaStyles.fontFamily
                            }

                            validator: IntValidator { bottom:0 }

                            onTextChanged: messageFeeds.controller.locationBroadcastFrequency = Number(text)
                        }
                    }

                    Label {
                        text: "General"
                        font {
                            family: DsaStyles.fontFamily
                            underline: true
                            pixelSize: DsaStyles.titleFontPixelSize * 0.75
                        }
                        color: Material.foreground
                    }

                    // Change the default coordinate formats between DMS, USNG, MGRS, etc.
                    Row {
                        width: parent.width
                        spacing: 10 * scaleFactor

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Default Coordinate Format"
                            font {
                                pixelSize: 12 * scaleFactor
                                family: DsaStyles.fontFamily
                            }
                            color: Material.foreground
                        }

                        ComboBox {
                            anchors.verticalCenter: parent.verticalCenter
                            model: optionsController.coordinateFormats
                            Component.onCompleted: currentIndex = optionsController.initialFormatIndex
                            onCurrentTextChanged: {
                                optionsController.setCoordinateFormat(currentText);
                                gridController.coordinateFormat = currentText;
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 10 * scaleFactor

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Grid Color Scheme"
                            font {
                                pixelSize: 12 * scaleFactor
                                family: DsaStyles.fontFamily
                            }
                            color: Material.foreground
                        }

                        ComboBox {
                            anchors.verticalCenter: parent.verticalCenter
                            model: gridController.gridColorSchemes
                            Component.onCompleted: currentIndex = gridController.gridColorSchemeIndex
                            onCurrentTextChanged: {
                                gridController.gridColorScheme = currentText;
                            }
                        }
                    }

                    // Change the default units between feet and meters
                    Row {
                        width: parent.width
                        spacing: 10 * scaleFactor

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Default Unit of Measurement"
                            font {
                                pixelSize: 12 * scaleFactor
                                family: DsaStyles.fontFamily
                            }
                            color: Material.foreground
                        }

                        ComboBox {
                            anchors.verticalCenter: parent.verticalCenter
                            model: optionsController.units
                            Component.onCompleted: currentIndex = optionsController.initialUnitIndex
                            onCurrentTextChanged: optionsController.setUnitOfMeasurement(currentText)
                        }
                    }

                    Row {
                        height: 40 * scaleFactor
                        spacing: 5 * scaleFactor

                        Text {
                            text: "User name"
                            color: Material.foreground
                            font {
                                pixelSize: 10 * scaleFactor
                                family: DsaStyles.fontFamily
                            }
                        }

                        TextField {
                            width: 128 * scaleFactor
                            text: optionsController.userName
                            color: Material.foreground
                            font {
                                pixelSize: 10 * scaleFactor
                                family: DsaStyles.fontFamily
                            }

                            onTextEdited: {
                                if (optionsController.userName !== text)
                                    optionsController.userName = text;
                            }
                        }
                    }
                }
            }
        }
        // Create a flickable column so that n number of options can be added
        Item {
            Flickable {
                id: configurationsFlickable
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }
                clip: true

                Component {
                    id: configurationListItemDelegate

                    Rectangle {
                        color: Material.backgroundColor
                        height: 40 * scaleFactor
                        width: parent.width

                        ProgressBar {
                            id: progressBarPercentComplete
                            anchors {
                                bottom: parent.bottom
                                right: parent.right
                                left: parent.left
                            }
                            visible: model.InProgress
                            to: 100
                            value: model.PercentComplete
                        }

                        RadioButton {
                            id: checkboxSelected
                            anchors {
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                            }
                            checked: model.Selected
                            enabled: model.Downloaded && model.Extracted
                            onClicked: {
                                configurationController.select(index);
                                checked = Qt.binding(function () { // restore the binding
                                    return model.Selected;
                                });
                            }
                        }

                        Label {
                            id: labelName
                            anchors {
                                left: checkboxSelected.right
                                verticalCenter: parent.verticalCenter
                            }
                            height: parent.height
                            text: model.Name
                            verticalAlignment: Text.AlignVCenter
                        }

                        Label {
                            id: labelRequiresRestart
                            anchors {
                                left: labelName.right
                                verticalCenter: parent.verticalCenter
                                margins: 2
                            }
                            text: "*"
                            color: "yellow"
                            font.italic: true
                            visible: model.RequiresRestart
                        }

                        CalciteIcon {
                            id: imageCancel
                            iconName: CalciteIcons.Calcite_x
                            height: parent.height
                            width: visible ? height : 0
                            visible: model.Downloading && model.IsCancellable
                            anchors {
                                right: imageDownload.left
                                verticalCenter: parent.verticalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    configurationController.cancel(index);
                                }
                            }
                        }

                        CalciteIcon {
                            id: imageDownload
                            iconName: CalciteIcons.Calcite_download
                            height: parent.height
                            visible: model.CanDownload
                            width: visible ? height : 0
                            enabled: model.CanDownload
                            anchors {
                                right: imageRemove.left
                                verticalCenter: parent.verticalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    configurationController.download(index);
                                }
                            }
                        }

                        CalciteIcon {
                            id: imageRemove
                            iconName: CalciteIcons.Calcite_trash
                            height: parent.height
                            width: visible ? height : 0
                            visible: model.CanDelete
                            anchors {
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    configurationDialogConfirmRemove.configurationName = model.Name
                                    configurationDialogConfirmRemove.alsoRemoveEntry = !model.Downloaded
                                    var msg = "Remove '" + model.Name + "' " + (model.Downloaded ? "files from the device?" : "from the list?");
                                    configurationDialogConfirmRemove.confirmationMessage = msg;
                                    configurationDialogConfirmRemove.open();
                                }
                            }
                        }
                    }
                }
                Label {
                    text: "Changing a configuration will require restarting the application"
                    id: labelRequiresRestart
                    width: parent.width
                    wrapMode: "WordWrap"
                    font {
                        family: DsaStyles.fontFamily
                        pixelSize: DsaStyles.titleFontPixelSize * 0.75
                        italic: true
                    }
                }

                ListView {
                    id: configurationList
                    anchors {
                        top: labelRequiresRestart.bottom
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }
                    interactive: true
                    clip: true
                    spacing: 2 * scaleFactor
                    model: configurationController.configurations
                    delegate: configurationListItemDelegate
                }

                Rectangle {
                    color: Material.accent
                    radius: 5 * scaleFactor
                    height: 40 * scaleFactor
                    width: height
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                    }
                    CalciteIcon {
                        id: imageAddConfiguration
                        iconName: CalciteIcons.Calcite_browser_plus
                        height: parent.height - 5 * scaleFactor
                        width: parent.width - 5 * scaleFactor
                        color: "black"
                        anchors {
                            verticalCenter: parent.verticalCenter
                            horizontalCenter: parent.horizontalCenter
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                toolRect.state = "add configuration";
                                drawer.open();
                            }
                        }
                    }
                }
            }

            Button {
                id: buttonCloseApp
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: 5
                }
                onClicked: {
                    showCloseDialog("Are you sure you want to close?");
                }

                text: "Close App"
                visible: configurationController.requiresRestart
            }
        }
    }
    Button {
        id: buttonDismiss
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 10 * scaleFactor
        }
        text: "Dismiss"
        onClicked: {
            optionsRoot.visible = false;
        }
    }

    Dialog {
        id: configurationDialogConfirmRemove
        anchors.centerIn: parent
        title: "Confirm Remove"
        standardButtons: Dialog.Yes | Dialog.No
        property alias confirmationMessage: configurationDialogLabel.text
        property string configurationName: ""
        property bool alsoRemoveEntry: false
        Label {
            id: configurationDialogLabel
            font {
                pixelSize: 12 * scaleFactor
                family: DsaStyles.fontFamily
            }
            wrapMode: Text.Wrap
            width: parent.width
        }

        onAccepted: configurationController.remove(configurationName, alsoRemoveEntry);
    }

    Authenticator {
        anchors.centerIn: parent
    }
}
