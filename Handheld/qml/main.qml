
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
import Esri.Handheld 1.0

Handheld {
    width: 320
    height: 480

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    // Create SceneQuickView here, and create its Scene etc. in C++ code
    SceneView {
        anchors.fill: parent
        objectName: "sceneView"
    }

    Column {
        anchors{
            margins: 2 * scaleFactor
            top: parent.top
            right: parent.right
        }
        spacing: 5 * scaleFactor

        Button {
            id: basemapsCheckBox
            width: 32 * scaleFactor
            height: 32 * scaleFactor

            background: Rectangle {
                anchors.fill: basemapsCheckBox
                color: Material.primary
            }

            Image {
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
                sourceSize.height: basemapsCheckBox.background.height - (6 * scaleFactor)
                height: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png"
            }

            onClicked: basemapsTool.slideVertical(basemapsTool.height, 0);
        }        

        Button {
            id: addLocalDataCheckBox
            width: 32 * scaleFactor
            height: 32 * scaleFactor

            background: Rectangle {
                anchors.fill: addLocalDataCheckBox
                color: Material.primary
            }

            Image {
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
                sourceSize.height: addLocalDataCheckBox.background.height - (6 * scaleFactor)
                height: sourceSize.height
                source: "qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png"
            }

            onClicked: addLocalDataTool.slideVertical(addLocalDataTool.height, 0);
        }
    }

    BasemapPicker {
        id: basemapsTool
        anchors {
            left: parent.left
            right: parent.right
        }
        height: parent.height
        y: height
        onBasemapSelected: closed()
        onClosed: slideVertical(0, height)
    }

    AddLocalData {
        id: addLocalDataTool
        anchors {
            left: parent.left
            right: parent.right
        }
        y: height
        height: parent.height
        showDataConnectionPane: false
        onClosed: slideVertical(0, height)
    }
}
