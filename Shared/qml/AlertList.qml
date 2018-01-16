
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
    id: alertsRoot
    width: 272 * scaleFactor
    title: qsTr("Alerts")
    clip: true

    AlertToolController {
        id: toolController
    }

    ComboBox {
        id: statusFilter
        anchors {
            top: alertsRoot.titleBar.bottom
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        model: ["Inactive", "Low", "Medium", "High", "Critical"]
        currentIndex: 0
        onCurrentIndexChanged: toolController.setMinStatus(currentIndex);
    }

    ListView {
        id: alertsView
        anchors {
            top: statusFilter.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        model: toolController.alertListModel
        clip: true

        delegate: SwipeDelegate {
            width: parent.width
            Text {
                text: message + ": " + (status === 0 ?
                                            "inactive" : (status === 1 ?
                                                              "low" : (status === 2 ?
                                                                           "medium" : (status === 3 ?
                                                                                           "high" : (status === 4 ?
                                                                                                         "critical" : "???")))))
                color: "white"

                MouseArea {
                    anchors.fill: parent

                    onClicked: toolController.highlight(index);
                    onDoubleClicked: toolController.zoomTo(index);
                }
            }

            swipe.right: Label {
                id: deleteLabel
                text: qsTr("Dismiss")
                color: "red"
                verticalAlignment: Label.AlignVCenter
                padding: 12
                height: parent.height
                anchors.right: parent.right

                SwipeDelegate.onClicked: toolController.dismiss(index);

                background: Rectangle {
                    color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                }
            }

            Component.onCompleted: {
                if (visible)
                    toolController.setViewed(index);
            }
        }
    }

    onVisibleChanged: {
        if (!visible)
            return;

        for (var i = 0; i < alertsView.count; ++i)
            toolController.setViewed(i);
    }
}
