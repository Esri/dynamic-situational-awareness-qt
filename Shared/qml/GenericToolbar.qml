
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

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

ToolBar {
    property alias toolbarLabelText: toolbarLabel.text
    property int fontSize: 24 * scaleFactor
    property int marginWidth: 5 * scaleFactor
    property int imageWidth: 36 * scaleFactor
    property int toolbarHeight: 42 * scaleFactor
    signal menuClicked();
    signal drawerClicked();
//    background: Rectangle {
//        implicitHeight: toolbarHeight
//        color: Material.background
//    }

    height: toolbarHeight

    RowLayout {
        anchors.fill: parent

        //        Image {
        //            anchors {
        //                left: parent.left
        //                verticalCenter: parent.verticalCenter
        //                margins: 5
        //            }
        //            source: "qrc:/Resources/icons/xhdpi/drawer_large.png"
        //            width: 36
        //            height: width

        //            MouseArea {
        //                anchors.fill: parent
        //                onClicked: {
        //                    // emit clicked
        //                    drawerClicked();
        //                }
        //            }
        //        }

        Label {
            id: toolbarLabel
            anchors {
                left: parent.left
                leftMargin: marginWidth * 2
                verticalCenter: parent.verticalCenter
            }

            font {
                pixelSize: fontSize
            }
            color: Material.foreground
        }

        Image {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: marginWidth
            }
            source: "qrc:/Resources/icons/xhdpi/menu_large.png"
            width: imageWidth
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // emit clicked
                    menuClicked();
                }
            }
        }
    }
}

