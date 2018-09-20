/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    id: rootMarkup
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    // expose properties to be used by other tools
    property alias markupEnabled: markupController.drawModeEnabled

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
        onClearDialogAccepted: markupController.deleteAllGraphics();
        onInputDialogAccepted: {
            markupController.setOverlayName(input)
            markupController.shareMarkup();
        }
        onMarkupLayerReceived: {
            markupController.deleteAllGraphics();
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

        onActiveChanged: {
            if (!active)
                rootMarkup.visible = false;
        }

        onMarkupReceived: {
            markupDialog.title = "Markup Received";
            markupDialog.path = filePath;
            markupDialog.informativeText = "%1 has sent you a markup. Would you like to view it now?".arg(sharedBy)
            markupDialog.open();
        }

        onMarkupSent: {
            markupDialog.title = "Markup Shared";
            markupDialog.path = filePath;
            markupDialog.informativeText = "The shared markup has been added as an overlay. Would you like to view it now?";
            markupDialog.open();
        }
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
                iconSource: DsaResources.iconSendMap
                toolName: "Share"
                onToolSelected: appRoot.showInputDialog("Share Markup", "Markup name", "ex: Markup 1");
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
            width: parent.width
            spacing: 8 * scaleFactor
            currentIndex: 0
            clip: true
            snapMode: ListView.SnapOneItem

            delegate: Component {

                Rectangle {
                    height: DsaStyles.mainToolbarHeight * 0.35
                    width: height
                    radius: 100 * scaleFactor
                    color: markupController.colors[index]

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
    }

    Item {
        id: markupToc
        anchors.fill: parent

        // TODO
    }

    // calls into C++ to create a new SimpleLineSymbol with the desired color
    function selectColor(colorRectangle) {
        colorModel.setProperty(colorView.currentIndex, "selected", false);
        markupController.setColor(colorRectangle.color);
    }

    // initialize the ListModel with the initial draw colors
    Component.onCompleted: {
        var colors = markupController.colors;
        for (var i = 0; i < colors.length; i++)
            colorModel.append({"drawColor": colors[i], "selected": false});

        markupController.setColor(colors[0]);
        colorModel.setProperty(colorView.currentIndex, "selected", true);
    }
}
