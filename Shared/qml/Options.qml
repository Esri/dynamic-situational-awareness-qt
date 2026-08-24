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
    property real settingsFontSize: 14 * scaleFactor
    property real settingsFieldHeight: 36 * scaleFactor
    property real settingsRowSpacing: 10 * scaleFactor
    property real settingsLabelMinWidth: 220 * scaleFactor
    property real settingsControlWidth: 150 * scaleFactor

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
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: "Settings are saved automatically"
                        color: Material.foreground
                        font {
                            family: DsaStyles.fontFamily
                            italic: true
                            pixelSize: settingsFontSize * 0.85
                        }
                    }

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
                        font {
                            pixelSize: settingsFontSize
                            family: DsaStyles.fontFamily
                        }
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
                        font {
                            pixelSize: settingsFontSize
                            family: DsaStyles.fontFamily
                        }
                        checked: true
                        onCheckedChanged: {
                            // update visibility of UI component
                            currentLocation.visible = checked;
                        }
                    }

                    // Toggle friendly tracks labels
                    CheckBox {
                        text: "Show friendly tracks labels"
                        font {
                            pixelSize: settingsFontSize
                            family: DsaStyles.fontFamily
                        }
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
                        font {
                            pixelSize: settingsFontSize
                            family: DsaStyles.fontFamily
                        }
                        checked: optionsController.useGpsForElevation
                        onCheckedChanged: optionsController.useGpsForElevation = checked
                    }


                    CheckBox {
                        text: "Location Broadcast"
                        font {
                            pixelSize: settingsFontSize
                            family: DsaStyles.fontFamily
                        }
                        checked: messageFeeds.controller.locationBroadcastEnabled
                        onCheckedChanged: messageFeeds.controller.locationBroadcastEnabled = checked
                    }

                    RowLayout {
                        width: parent.width
                        spacing: settingsRowSpacing

                        Label {
                            Layout.preferredWidth: settingsLabelMinWidth
                            Layout.alignment: Qt.AlignVCenter
                            text: "Location Broadcast frequency (ms)"
                            color: Material.foreground
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }
                        }

                        TextField {
                            Layout.preferredWidth: settingsControlWidth/2
                            Layout.alignment: Qt.AlignVCenter
                            implicitHeight: settingsFieldHeight
                            text: messageFeeds.controller.locationBroadcastFrequency
                            color: Material.foreground
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }

                            validator: IntValidator { bottom:0 }

                            onTextChanged: messageFeeds.controller.locationBroadcastFrequency = Number(text)
                        }

                        Item {
                            Layout.fillWidth: true
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
                    RowLayout {
                        width: parent.width
                        spacing: settingsRowSpacing

                        Label {
                            Layout.preferredWidth: Math.min(settingsLabelMinWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            text: "Default Coordinate Format"
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }
                            color: Material.foreground
                        }

                        Item {
                            Layout.preferredWidth: Math.min(settingsControlWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            implicitHeight: settingsFieldHeight

                            ComboBox {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.min(parent.width, settingsControlWidth * 0.75)
                                implicitHeight: settingsFieldHeight
                                font {
                                    pixelSize: settingsFontSize
                                    family: DsaStyles.fontFamily
                                }
                                model: optionsController.coordinateFormats
                                Component.onCompleted: currentIndex = optionsController.initialFormatIndex
                                onCurrentTextChanged: {
                                    optionsController.setCoordinateFormat(currentText);
                                    gridController.coordinateFormat = currentText;
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: settingsRowSpacing

                        Label {
                            Layout.preferredWidth: Math.min(settingsLabelMinWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            text: "Grid Color Scheme"
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }
                            color: Material.foreground
                        }

                        Item {
                            Layout.preferredWidth: Math.min(settingsControlWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            implicitHeight: settingsFieldHeight

                            ComboBox {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.min(parent.width, settingsControlWidth * 0.75)
                                implicitHeight: settingsFieldHeight
                                font {
                                    pixelSize: settingsFontSize
                                    family: DsaStyles.fontFamily
                                }
                                model: gridController.gridColorSchemes
                                Component.onCompleted: currentIndex = gridController.gridColorSchemeIndex
                                onCurrentTextChanged: {
                                    gridController.gridColorScheme = currentText;
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    // Change the default units between feet and meters
                    RowLayout {
                        width: parent.width
                        spacing: settingsRowSpacing

                        Label {
                            Layout.preferredWidth: Math.min(settingsLabelMinWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            text: "Default Unit of Measurement"
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }
                            color: Material.foreground
                        }

                        Item {
                            Layout.preferredWidth: Math.min(settingsControlWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            implicitHeight: settingsFieldHeight

                            ComboBox {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.min(parent.width, settingsControlWidth * 0.75)
                                implicitHeight: settingsFieldHeight
                                font {
                                    pixelSize: settingsFontSize
                                    family: DsaStyles.fontFamily
                                }
                                model: optionsController.units
                                Component.onCompleted: currentIndex = optionsController.initialUnitIndex
                                onCurrentTextChanged: optionsController.setUnitOfMeasurement(currentText)
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: settingsRowSpacing

                        Label {
                            Layout.preferredWidth: Math.min(settingsLabelMinWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            text: "User name"
                            color: Material.foreground
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }
                        }

                        TextField {
                            Layout.preferredWidth: Math.min(settingsControlWidth, parent.width * 0.5)
                            Layout.alignment: Qt.AlignVCenter
                            implicitHeight: settingsFieldHeight
                            text: optionsController.userName
                            color: Material.foreground
                            font {
                                pixelSize: settingsFontSize
                                family: DsaStyles.fontFamily
                            }

                            onTextEdited: {
                                if (optionsController.userName !== text)
                                    optionsController.userName = text;
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
        // Create a flickable column so that n number of options can be added
        Item {
            clip: true

            Flickable {
                id: configurationsFlickable
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                    bottomMargin: 10 * scaleFactor + buttonCloseApp.height
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

                        Image {
                            id: imageCancel
                            source: "qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png"
                            height: parent.height
                            width: model.Downloading && model.IsCancellable ? parent.height : 0
                            anchors {
                                right: imageDownload.left
                                verticalCenter: parent.verticalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    configurationController.cancel(index);
                                    if (timerDebounce.running)
                                        imageDownload.visible = true;
                                }
                            }
                        }

                        property bool downloadOnCooldown: false
                        Timer {
                            id: timerDebounce
                            interval: 500
                            running: false
                            repeat: false
                            onTriggered: {
                                downloadOnCooldown = false;
                                imageDownload.visible = true;
                            }
                        }
                        Image {
                            id: imageDownload
                            source: "qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png"
                            height: parent.height
                            width: model.CanDownload ? parent.height : 0
                            enabled: model.CanDownload
                            anchors {
                                right: imageRemove.left
                                verticalCenter: parent.verticalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (downloadOnCooldown)
                                        return;

                                    downloadOnCooldown = true;
                                    imageDownload.visible = false;
                                    configurationController.download(index);
                                    timerDebounce.start();
                                }
                            }
                            rotation: 180
                        }

                        Image {
                            id: imageRemove
                            source: DsaResources.iconTrash
                            height: parent.height
                            width: model.CanDelete ? parent.height : 0
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
                    Image {
                        id: imageAddConfiguration
                        source: "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"
                        height: parent.height - 5 * scaleFactor
                        width: parent.width - 5 * scaleFactor
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
