// Copyright 2017 ESRI
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

Item {
    property bool itemVisible
    property url imageUrl
    property bool imageVisible
    property string mainText

    Row {
        spacing: 3 * scaleFactor
        CheckBox {
            id: visibleCheckBox
            anchors.verticalCenter: parent.verticalCenter
            checked: itemVisible
            onClicked: itemVisible = checked;
        }

        Rectangle {
            id: iconImage
            anchors.verticalCenter: parent.verticalCenter
            radius: 50 * scaleFactor
            width: 24 * scaleFactor
            height: width
            color: Material.foreground
            visible: imageVisible

            Image {
                anchors {
                    fill: parent
                    margins: 2 * scaleFactor
                }
                source: imageUrl
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: mainText
            verticalAlignment: Text.AlignVCenter
            color: Material.foreground
            font.pixelSize: 14 * scaleFactor
            font.bold: true
        }
    }

    //    Image {
    //        id: zoomToImage
    //        fillMode: Image.PreserveAspectFit
    //        anchors{
    //            right: parent.right
    //            verticalCenter: parent.verticalCenter
    //        }
    //        sourceSize.height: 32 * scaleFactor
    //        height: sourceSize.height
    //        source: DsaResources.iconZoomTo

    //        MouseArea {
    //            anchors.fill: parent

    //            onClicked: {
    //                toolController.zoomTo(index);
    //                mapToolRow.tocIconSelected = false;
    //                tocRoot.visible = false;
    //            }
    //        }
    //    }
}
