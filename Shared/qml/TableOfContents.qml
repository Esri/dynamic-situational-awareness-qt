
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaToolBase {
    id: tocRoot

    title: "Add Local Data"
    width: 272 * scaleFactor

    // Create the controller
    TableOfContentsController {
        id: toolController
    }

    // Declare the ListView, which will display the list of files
    ListView {
        id: layersList

        anchors {
            top: tocRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }

        clip: true
        model: toolController.layerListModel
        width: parent.width
        delegate: Row {
            anchors.margins: 4 * scaleFactor
            width: layersList.width
            height: 32 * scaleFactor
            spacing: 8 * scaleFactor

            CheckBox {
                height: parent.height
                checked: layerVisible
                onClicked: layerVisible = checked;
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                height: parent.height
                width: 64 * scaleFactor
                text: name
            }

            Image {
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.height: parent.height - (6 * scaleFactor)
                height: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_zoomtomapextent_light.png"

                MouseArea {
                    anchors.fill: parent

                    onClicked: toolController.zoomTo(index);
                }
            }


            Image {
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.height: parent.height - (6 * scaleFactor)
                height: sourceSize.height
                width: 32 * scaleFactor
                source: index > 0 ?
                            "qrc:/Resources/icons/xhdpi/ic_menu_spinneractive_light.png" :
                            ""

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (index > 0)
                            toolController.moveUp(index);
                    }
                }
            }

            Image {
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.height: parent.height - (6 * scaleFactor)
                height: sourceSize.height
                width: 32 * scaleFactor
                source: index < (layersList.count -1) ?
                            "qrc:/Resources/icons/xhdpi/ic_menu_spinneridle_light.png" :
                            ""

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (index < (layersList.count -1))
                            toolController.moveDown(index);
                    }
                }
            }

            Image {
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.height: parent.height - (6 * scaleFactor)
                height: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_trash_light.png"

                MouseArea {
                    anchors.fill: parent

                    onClicked: toolController.removeAt(index);
                }
            }
        }
    }
}

