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
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0

Rectangle {
    id: rootMarkup
    //title: qsTr("Markup")
    color: Material.primary
    opacity: 0.85

    // Modifying this array will change the initial available colors
    property var drawColors: ["#000000", "#ffffff", "#F44336", "#03a9f4", "#fff176"]

    // state strings
    property string drawState: "draw"
    property string editState: "edit"
    property string tocState: "tocState"

    // to be emitted for UI purposes
    signal graphicsDeleted()
    signal colorSelected()
    signal colorDialogVisibleChanged(bool dialogVisible)

    MarkupController {
        id: markupController
        drawModeEnabled: true
    }

    state: drawState
    states: [
        State {
            name: drawState
            PropertyChanges {
                target: rootMarkup
                visible: false
            }
        },
        State {
            name: editState
            PropertyChanges {
                target: editRow
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: 250 * scaleFactor
            }
            PropertyChanges {
                target: rootMarkup
                height: DsaStyles.mainToolbarHeight
            }
        },
        State {
            name: tocState
            PropertyChanges {
                target: editRow
                visible: false
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: drawer.width
            }
            PropertyChanges {
                target: rootMarkup
                height: drawer.height
            }
        }
    ]

    Row {
        id: editRow
        anchors.fill: parent
        visible: false

        Text {
            text: "edit"
        }

        Text {
            text: "edit2"
        }
    }

//    Column {
//        id: menuColumn
//        spacing: 15 * scaleFactor
//        anchors {
//            top: titleBar.bottom
//            horizontalCenter: parent.horizontalCenter
//        }
//        topPadding: 15 * scaleFactor
//        width: parent.width / 1.1

//        // color selection
//        Item {
//            width : parent.width
//            height: 75 * scaleFactor

//            DropShadow {
//                anchors.fill: colorRect
//                horizontalOffset: -1
//                verticalOffset: 1
//                radius: 8 * scaleFactor
//                smooth: true
//                samples: 16
//                color: "#80000000"
//                source: colorRect
//            }

//            Rectangle {
//                id: colorRect
//                anchors.fill: parent
//                color: Material.background

//                Text {
//                    id: colorTitle
//                    anchors {
//                        top: parent.top
//                        left: parent.left
//                        margins: 5 * scaleFactor
//                    }
//                    text: qsTr("Draw Color")
//                    color: Material.foreground
//                }

//                ListView {
//                    id: colorView
//                    anchors {
//                        top: colorTitle.bottom
//                        horizontalCenter: parent.horizontalCenter
//                        margins: 2 * scaleFactor
//                    }
//                    orientation: ListView.Horizontal
//                    model: colorModel
//                    height: 30 * scaleFactor
//                    width: 150 * scaleFactor
//                    spacing: 5 * scaleFactor
//                    currentIndex: 0
//                    snapMode: ListView.SnapOneItem

//                    delegate: Component {

//                        Rectangle {
//                            height: 30 * scaleFactor
//                            width: height
//                            radius: 100 * scaleFactor
//                            color: drawColors[index]
//                            border {
//                                color: Material.accent
//                                width: 0.50 * scaleFactor
//                            }

//                            Image {
//                                anchors.centerIn: parent
//                                height: parent.height
//                                width: height
//                                source: DsaResources.iconComplete
//                                visible: selected
//                            }

//                            MouseArea {
//                                anchors.fill: parent
//                                onClicked: {
//                                    selectColor(parent);
//                                    parent.ListView.view.currentIndex = index;
//                                    selected = true;

//                                    if (markupController.drawModeEnabled)
//                                        colorSelected();
//                                }
//                            }
//                        }
//                    }

//                    ListModel {
//                        id: colorModel
//                    }
//                }

//                // button for adding new colors
//                RoundButton {
//                    id: addButton
//                    anchors {
//                        margins: 5 * scaleFactor
//                        right: parent.right
//                        bottom: parent.bottom
//                    }
//                    visible: Qt.platform.os !== "android" // ColorDialog does not scale properly on Android
//                    height: 20 * scaleFactor
//                    width: height
//                    opacity: 0.95

//                    background: Rectangle {
//                        implicitWidth: parent.width
//                        implicitHeight: implicitWidth
//                        opacity: enabled ? 1 : 0.3
//                        radius: addButton.radius
//                        color: Material.accent

//                        Image {
//                            anchors.centerIn: parent
//                            width: 16 * scaleFactor
//                            height: width
//                            source: DsaResources.iconAdd
//                        }
//                    }

//                    onClicked: {
//                        if (!newColorDialog.visible) {
//                            colorDialogVisibleChanged(true);
//                            newColorDialog.open();
//                        }
//                    }
//                }
//            }
//        }

//        // draw and layer options
//        Item {
//            width : parent.width
//            height: toolOptionsTitle.height + togglesColumn.height + 20 * scaleFactor

//            DropShadow {
//                anchors.fill: toggleRect
//                horizontalOffset: -1
//                verticalOffset: 1
//                radius: 8 * scaleFactor
//                smooth: true
//                samples: 16
//                color: "#80000000"
//                source: toggleRect
//            }

//            Rectangle {
//                id: toggleRect
//                anchors.fill: parent
//                color: Material.background

//                Text {
//                    id: toolOptionsTitle
//                    text: "Tool Options"
//                    color: Material.foreground
//                    anchors {
//                        top: parent.top
//                        left: parent.left
//                        margins: 5 * scaleFactor
//                    }
//                }

//                Column {
//                    id: togglesColumn
//                    anchors {
//                        top: toolOptionsTitle.bottom
//                        horizontalCenter: parent.horizontalCenter
//                        margins: 2 * scaleFactor
//                    }
//                    width: parent.width
//                    spacing: 1 * scaleFactor

//                    SwitchDelegate{
//                        id: layerAppendedSwitch
//                        text: qsTr("Visible")
//                        checked: markupController.active
//                        width: parent.width
//                        font.pixelSize: 15 * scaleFactor

//                        contentItem: Text {
//                            rightPadding: layerAppendedSwitch.indicator.width + layerAppendedSwitch.spacing
//                            text: layerAppendedSwitch.text
//                            font: layerAppendedSwitch.font
//                            opacity: enabled ? 1.0 : 0.3
//                            color: Material.foreground
//                            elide: Text.ElideRight
//                            horizontalAlignment: Text.AlignLeft
//                            verticalAlignment: Text.AlignVCenter
//                        }

//                        onCheckedChanged: {
//                            markupController.active = checked;
//                            if (!checked)
//                                drawModeSwitch.checked = checked;
//                        }
//                    }

//                    SwitchDelegate {
//                        id: drawModeSwitch
//                        text: qsTr("Drawing Mode Enabled")
//                        checked: markupController.drawModeEnabled
//                        width: parent.width
//                        font.pixelSize: 15 * scaleFactor

//                        contentItem: Text {
//                            rightPadding: drawModeSwitch.indicator.width + drawModeSwitch.spacing
//                            text: drawModeSwitch.text
//                            font: drawModeSwitch.font
//                            opacity: enabled ? 1.0 : 0.3
//                            color: Material.foreground
//                            elide: Text.ElideRight
//                            horizontalAlignment: Text.AlignLeft
//                            verticalAlignment: Text.AlignVCenter
//                        }

//                        onCheckedChanged: {
//                            markupController.drawModeEnabled = checked;
//                            if (checked)
//                                layerAppendedSwitch.checked = checked;
//                        }
//                    }

//                    Label {
//                        text: "Surface Placement"
//                        leftPadding: 5 * scaleFactor
//                        color: Material.foreground
//                        visible: markupController.is3d
//                    }

//                    Row {
//                        anchors.horizontalCenter: parent.horizontalCenter
//                        width: parent.width
//                        padding: 5 * scaleFactor
//                        spacing: 5 * scaleFactor

//                        ComboBox {
//                            id: surfacePlacementComboBox
//                            model: ["Draped", "Relative"]
//                            currentIndex: 0
//                            width: currentIndex === 0 ? parent.width * 0.95 : parent.width / 2
//                            visible: markupController.is3d

//                            onCurrentIndexChanged: {
//                                // Draped
//                                if (currentIndex === 0)
//                                    markupController.setSurfacePlacement(currentIndex);
//                                // The corresponding Enum value for Relative placement is 2
//                                else if (currentIndex === 1)
//                                    markupController.setSurfacePlacement(currentIndex + 1);
//                            }

//                            Behavior on width {
//                                SpringAnimation {
//                                    spring: 3
//                                    damping: .4
//                                }
//                            }
//                        }

//                        TextField {
//                            id: altitudeInput
//                            anchors.verticalCenter: parent.verticalCenter
//                            visible: surfacePlacementComboBox.currentIndex === 1
//                            placeholderText: "Alt. (m)"
//                            width: parent.width / 3
//                            color: Material.foreground
//                            font.pixelSize: 12 * scaleFactor
//                            inputMethodHints: Qt.ImhFormattedNumbersOnly


//                            onTextChanged: {
//                                markupController.drawingAltitude = Number(text)
//                            }
//                        }
//                    }
//                }
//            }
//        }

//        Item {
//            width : parent.width
//            height: graphicsRemoveTitle.height + removeGraphicsColumn.height + 10 * scaleFactor

//            DropShadow {
//                anchors.fill: graphicDeleteRect
//                horizontalOffset: -1
//                verticalOffset: 1
//                radius: 8 * scaleFactor
//                smooth: true
//                samples: 16
//                color: "#80000000"
//                source: graphicDeleteRect
//            }

//            Rectangle {
//                id: graphicDeleteRect
//                anchors.fill: parent
//                color: Material.background

//                Text {
//                    id: graphicsRemoveTitle
//                    text: "Remove Graphics"
//                    color: Material.foreground
//                    anchors {
//                        top: parent.top
//                        left: parent.left
//                        margins: 5 * scaleFactor
//                    }
//                }

//                Column {
//                    id: removeGraphicsColumn
//                    anchors{
//                        top: graphicsRemoveTitle.bottom
//                        horizontalCenter: parent.horizontalCenter
//                        margins: 2 * scaleFactor
//                    }
//                    width: parent.width
//                    spacing: 2 * scaleFactor

//                    Button {
//                        anchors.horizontalCenter: parent.horizontalCenter
//                        text: "Selected Graphics"
//                        width: parent.width / 1.50
//                        onClicked: {
//                            graphicsDeleted()
//                            markupController.deleteSelectedGraphics()
//                        }
//                    }

//                    Button {
//                        anchors.horizontalCenter: parent.horizontalCenter
//                        text: "All Graphics"
//                        width: parent.width / 1.50
//                        onClicked: {
//                            graphicsDeleted()
//                            markupController.deleteAllGraphics()
//                        }
//                    }
//                }
//            }
//        }
//    }

//    ColorDialog {
//        id: newColorDialog
//        title: "Choose a Draw Color"

//        onAccepted: {
//            colorDialogVisibleChanged(false);
//            drawColors.push(color);
//            colorModel.append({"selected": false});
//            colorView.positionViewAtEnd();
//        }
//    }

//    // calls into C++ to create a new SimpleLineSymbol with the desired color
//    function selectColor(colorRectangle) {
//        colorModel.setProperty(colorView.currentIndex, "selected", false);
//        markupController.setColor(colorRectangle.color);
//    }

//    onVisibleChanged: {
//        if (visible)
//            markupController.active = true;
//        if (visible && !drawModeSwitch.checked) {
//            drawModeSwitch.checked = true;
//            markupController.drawModeEnabled = true;
//        }
//        if (!visible && drawModeSwitch.checked) {
//            drawModeSwitch.checked = false;
//            markupController.drawModeEnabled = false;
//        }
//    }

//    // initialize the ListModel with the initial draw colors
//    Component.onCompleted: {
//        for (var i = 0; i < drawColors.length; i++)
//            colorModel.append({"drawColor": drawColors[i], "selected": false});

//        markupController.setColor(drawColors[0]);
//        colorModel.setProperty(colorView.currentIndex, "selected", true);
//    }
}
