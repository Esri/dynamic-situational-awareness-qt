
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

Rectangle {    
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property bool showDataConnectionPane: true
    signal closed();

    AddLocalDataController {
        id: toolController
    }

    Column {
        id: titleBar
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
        }
        Rectangle {
            color: Material.primary
            height: 32 * scaleFactor
            width: parent.width

            Text {
                id: titleText
                anchors.centerIn: parent
                text: qsTr("Add Local Data")
                color: Material.foreground
                font.bold: true
            }

            Button {
                id: closeButton

                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                    margins: 2 * scaleFactor
                }


                width: height

                background: Rectangle {
                    anchors.fill: closeButton
                    color: Material.primary
                }

                Image {
                    anchors.fill: parent
                    source: "qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png"
                    fillMode: Image.PreserveAspectFit
                }

                onClicked: closed();
            }
        }
        Rectangle {
            //color: Material.primary
            height: 32 * scaleFactor
            width: parent.width
            visible: showDataConnectionPane
            Row {
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }

                ComboBox {
                    width: parent.width * 0.75
                }

                RoundButton {

                }
            }
        }
    }

    Component {
        id: localDataDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35 * scaleFactor

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: -8 * scaleFactor
                    leftMargin: 20 * scaleFactor
                    rightMargin: 20 * scaleFactor
                }
                color: "darkgrey"
                height: 1 * scaleFactor
            }

            Text {
                text: fileName
                anchors {
                    fill: parent
                    leftMargin: 5 * scaleFactor
                }
                elide: Text.ElideRight
                font.pixelSize: 14 * scaleFactor
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    toolController.selectItem(index);
                    closed();
                }
            }
        }
    }

    ListView {
        id: localDataList

        property string currentPath: ""

        anchors{
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }

        clip: true
        model: toolController.localDataModel
        width: parent.width
        delegate: localDataDelegate
    }
}
