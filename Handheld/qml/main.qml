
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

    GenericToolbar {
        id: toolbar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 42 * scaleFactor
        toolbarLabelText: "DSA - Handheld"

        onMenuClicked: {
            console.log("Menu button was clicked");
        }

        onDrawerClicked: {
            console.log("Drawer was clicked");
        }
    }

    // Create SceneQuickView here, and create its Scene etc. in C++ code
    SceneView {
        anchors {
            top: toolbar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        objectName: "sceneView"
    }

    Button {
        id: basemapsCheckBox
        anchors{
            margins: 2 * scaleFactor
            top: toolbar.bottom
            right: parent.right
        }
        checkable: true
        checked: false
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
    }

    BasemapPicker {
        id: basemapsTool

        anchors.fill: parent
        visible: basemapsCheckBox.checked

        onBasemapSelected: basemapsCheckBox.checked = false;
        onClosed: basemapsCheckBox.checked = false;
    }
}
