
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
import QtQuick.Controls 1.4
import Esri.Handheld 1.0

Handheld {
    width: 800
    height: 600

    // Create SceneQuickView here, and create its Scene etc. in C++ code
    SceneView {
        anchors.fill: parent
        objectName: "sceneView"
    }

    BasemapPickerTool {
        id: basemapsTool
        anchors {
            top: basemapsCheckBox.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: basemapsCheckBox.checked

        onBasemapSelected: {
            basemapsCheckBox.checked = false;
        }
    }

    Button {
        id: basemapsCheckBox
        anchors.top: parent.top
        anchors.right: parent.right
        checkable: true
        checked: false
        iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_light.png"
    }
}
