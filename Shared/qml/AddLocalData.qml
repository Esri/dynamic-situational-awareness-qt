
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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    id: localDataRoot
    property bool showDataConnectionPane: true
    property var selectedItems: []
    title: "Add Local Data"
    width: 272 * scaleFactor

    // Create the controller
    AddLocalDataController {
        id: toolController
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

    // When this button is clicked, all checked items will be added as layers/elevation sources
    RoundButton {
        id: addButton
        anchors {
            right: parent.right
            verticalCenter: localDataList.bottom
            margins: 10 * scaleFactor
        }
        opacity: 0.95

        background: Rectangle {
            implicitWidth: 40 * scaleFactor
            implicitHeight: 40 * scaleFactor
            opacity: enabled ? 1 : 0.3
            radius: addButton.radius
            color: Material.accent

            Image {
                anchors.centerIn: parent
                width: 26 * scaleFactor
                height: width
                source: "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"
            }

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
    }

    function resetView() {
        selectedItems = [];
        toolController.refreshLocalDataModel(filter.currentText);
    }
}
