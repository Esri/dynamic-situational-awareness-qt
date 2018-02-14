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
import QtQuick.Dialogs 1.2 as Dialogs1
import Esri.DSA 1.0

Item {
    id: rootMarkup

    // expose properties to be used by other tools
    property alias markupEnabled: markupController.drawModeEnabled

    // Modifying this array will change the initial available colors
    property var drawColors: ["#000000", "#ffffff", "#F44336", "#03a9f4", "#fff176"]

    // state strings
    property string drawState: "draw"
    property string colorState: "color"
    property string tocState: "toc"
    property string widthState: "width"
    property string clearState: "clear"

    // to be emitted for UI purposes
    signal graphicsDeleted()
    signal colorSelected()
    signal colorDialogVisibleChanged(bool dialogVisible)

    Connections {
        target: appRoot
        onClearDialogAccepted: markupController.clearGraphics()
        onInputDialogAccepted: {
            markupController.setName(input.length > 0 ? input : "sketch " + index);
            drawPane.sketchInProgress = false;
        }
    }

    onVisibleChanged: {
        if (visible) {
            state = drawState;
        } else {
            state = clearState;
        }
    }

    MarkupController {
        id: markupController

        onSketchCompleted: drawPane.sketchInProgress = true
        active: rootMarkup.visible
        drawModeEnabled: rootMarkup.visible
        sketching: drawPane.sketchInProgress
    }

    state: clearState
    states: [
        State {
            name: drawState
            PropertyChanges {
                target: markupController
                drawModeEnabled: true
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: 210 * scaleFactor
            }
            PropertyChanges {
                target: rootMarkup
                height: DsaStyles.mainToolbarHeight * scaleFactor
            }
            PropertyChanges {
                target: drawPane
                visible: sketchInProgress
            }
        },
        State {
            name: colorState
            PropertyChanges {
                target: colorPane
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: 210 * scaleFactor
            }
            PropertyChanges {
                target: rootMarkup
                height: DsaStyles.mainToolbarHeight * scaleFactor
            }
            PropertyChanges {
                target: markupController
                drawModeEnabled: false
            }
        },
        State {
            name: widthState
            PropertyChanges {
                target: widthPane
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                visible: true
            }
            PropertyChanges {
                target: rootMarkup
                width: 210 * scaleFactor
            }
            PropertyChanges {
                target: rootMarkup
                height: DsaStyles.mainToolbarHeight * scaleFactor
            }
            PropertyChanges {
                target: markupController
                drawModeEnabled: false
            }
        },
        State {
            name: tocState
            PropertyChanges {
                target: colorPane
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
            PropertyChanges {
                target: markupController
                drawModeEnabled: drawModeEnabled
            }
            PropertyChanges {
                target: markupToc
                visible: true
            }
        },
        State {
            name: clearState
            PropertyChanges {
                target: markupController
                drawModeEnabled: false
            }
            PropertyChanges {
                target: colorPane
                visible: false
            }
            PropertyChanges {
                target: drawPane
                visible: false
            }
            PropertyChanges {
                target: markupToc
                visible: false
            }
            PropertyChanges {
                target: rootMarkup
                visible: false
            }
            PropertyChanges {
                target: rootMarkup
                width: 0
            }
            PropertyChanges {
                target: rootMarkup
                height: 0
            }
        }
    ]

    SecondaryToolbar {
        id: drawPane
        property bool sketchInProgress: false

        Row {
            anchors.centerIn: parent
            spacing: 40 * scaleFactor
            visible: parent.visible

            ToolIcon {
                anchors.verticalCenter: parent.verticalCenter
                iconSource: DsaResources.iconComplete
                toolName: "Finish Sketch"
                onToolSelected: appRoot.showInputDialog("Sketch name", "ex: Sketch 1")
            }

            ToolIcon {
                anchors.verticalCenter: parent.verticalCenter
                iconSource: DsaResources.iconClose
                toolName: "Cancel Sketch"
                onToolSelected: {
                    markupController.clearCurrentSketch();
                    drawPane.sketchInProgress = false;
                }
            }
        }
    }

    SecondaryToolbar {
        id: widthPane

        Row {
            anchors.centerIn: parent
            spacing: 10 * scaleFactor
            visible: parent.visible
            width: parent.width

            Slider {
                id: widthSlider
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.75
                from: 1
                to: 20
                value: 8
                onValueChanged: markupController.setWidth(value)
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
                text: Math.round(widthSlider.value)
            }
        }
    }

    SecondaryToolbar {
        id: colorPane

        Text {
            id: colorTitle
            anchors {
                top: parent.top
                left: parent.left
                margins: 5 * scaleFactor
            }
            font {
                pixelSize: 10 * scaleFactor
                family: DsaStyles.fontFamily
            }
            text: qsTr("Draw Color")
            color: Material.foreground
        }

        ListView {
            id: colorView
            anchors {
                top: colorTitle.bottom
                left: parent.left
                margins: 5 * scaleFactor
            }
            orientation: ListView.Horizontal
            model: colorModel
            height: 25 * scaleFactor
            width: Qt.platform.os === "android" ? parent.width : 175 * scaleFactor
            spacing: 5 * scaleFactor
            currentIndex: 0
            clip: true
            snapMode: ListView.SnapOneItem

            delegate: Component {

                Rectangle {
                    height: DsaStyles.mainToolbarHeight * 0.45
                    width: height
                    radius: 100 * scaleFactor
                    color: drawColors[index]
                    border {
                        color: Material.accent
                        width: 0.50 * scaleFactor
                    }

                    Image {
                        anchors.centerIn: parent
                        height: parent.height
                        width: height
                        source: DsaResources.iconComplete
                        visible: selected
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            selectColor(parent);
                            parent.ListView.view.currentIndex = index;
                            selected = true;

                            if (markupController.drawModeEnabled)
                                colorSelected();

                            markup.state = drawState;
                            markupToolRow.drawSelected = true;
                            markupToolRow.configureSelected = false;
                        }
                    }
                }
            }

            ListModel {
                id: colorModel
            }
        }

        // button for adding new colors
        RoundButton {
            id: addButton
            anchors {
                margins: 5 * scaleFactor
                left: colorView.right
                top: colorTitle.bottom
            }
            visible: Qt.platform.os !== "android" // ColorDialog does not scale properly on Android
            height: 20 * scaleFactor
            width: height
            opacity: 0.95

            background: Rectangle {
                implicitWidth: parent.width
                implicitHeight: implicitWidth
                opacity: enabled ? 1 : 0.3
                radius: addButton.radius
                color: Material.accent

                Image {
                    anchors.centerIn: parent
                    width: 16 * scaleFactor
                    height: width
                    source: DsaResources.iconAdd
                }
            }

            onClicked: {
                if (!newColorDialog.visible) {
                    colorDialogVisibleChanged(true);
                    newColorDialog.open();
                }

            }
        }
    }

    Item {
        id: markupToc
        anchors.fill: parent

        // TODO
    }

    Dialogs1.ColorDialog {
        id: newColorDialog
        title: "Choose a Draw Color"

        onAccepted: {
            colorDialogVisibleChanged(false);
            drawColors.push(color);
            colorModel.append({"selected": false});
            colorView.positionViewAtEnd();
        }
    }

    // calls into C++ to create a new SimpleLineSymbol with the desired color
    function selectColor(colorRectangle) {
        colorModel.setProperty(colorView.currentIndex, "selected", false);
        markupController.setColor(colorRectangle.color);
    }

    // initialize the ListModel with the initial draw colors
    Component.onCompleted: {
        for (var i = 0; i < drawColors.length; i++)
            colorModel.append({"drawColor": drawColors[i], "selected": false});

        markupController.setColor(drawColors[0]);
        colorModel.setProperty(colorView.currentIndex, "selected", true);
    }
}
