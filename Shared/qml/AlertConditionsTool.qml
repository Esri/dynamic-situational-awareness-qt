
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

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

DsaPanel {
    id: manageAlertsRoot
    title: qsTr("Alert Conditions")
    clip: true

    property bool isMobile
    property alias pickMode: toolController.pickMode

    AlertConditionsController {
        id: toolController
        active: isMobile ? manageAlertsRoot.visible || pickMode : manageAlertsRoot.visible

        onActiveChanged: {
            if (!active && manageAlertsRoot.visible)
                manageAlertsRoot.visible = false;
        }
    }

    Text {
        id: conditionsListTitle
        anchors {
            margins: 8 * scaleFactor
            top: titleBar.bottom
            horizontalCenter: parent.horizontalCenter
        }
        text: qsTr("Conditions")
        color: Material.foreground
        font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }

    ListView {
        id: conditionsList
        property string currentName
        property int currentLevel
        clip: true
        anchors {
            margins: 8 * scaleFactor
            top: conditionsListTitle.bottom
            bottom: createNewConditionButton.top
            left: parent.left
            right: parent.right
        }
        highlight: Rectangle {
            radius: 5 * scaleFactor
            color: Material.accent
            opacity: 0.5
        }
        highlightFollowsCurrentItem: isMobile
        highlightMoveVelocity: 10000
        interactive: true
        model: toolController.conditionsList
        currentIndex: -1
        spacing: 8 * scaleFactor

        delegate: Item {
            width: conditionsList.width
            height: 64 * scaleFactor

            Column {
                width: parent.width
                height: parent.height

                ListItemDelegate {
                    id: conditionDelegate
                    width: parent.width
                    height: 40 * scaleFactor
                    itemChecked: conditionEnabled
                    menuIconVisible: true
                    imageUrl: level === 1 ?
                                  DsaResources.iconWarningGreen
                                : ( level === 2 ? DsaResources.iconWarningOrange
                                                : ( level === 3 ? DsaResources.iconWarningRed
                                                                : level === 4 ? DsaResources.iconWarningRedExclamation
                                                                              : "") )
                    imageVisible: true

                    onItemCheckedChanged: {
                        if (conditionEnabled !== itemChecked)
                            conditionEnabled = itemChecked;
                    }

                    mainText: conditionName

                    Image {
                        id: menuButton
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: 5 * scaleFactor
                        }
                        rotation: 90
                        source: DsaResources.iconMenu
                        height: 32 * scaleFactor
                        width: height

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                conditionsList.currentIndex = index;
                                if (!isMobile) {
                                    conditionMenu.open();
                                } else {
                                    if (mobileMenu.isOpen) {
                                        mobileMenu.close();
                                    } else {
                                        conditionsList.currentLevel = level;
                                        conditionsList.currentName = conditionName;
                                        mobileMenu.open();
                                    }
                                }
                            }
                        }

                        // Menu for Vehicle
                        Menu {
                            id: conditionMenu
                            width: 125 * scaleFactor

                            Column {
                                anchors.margins: 10 * scaleFactor
                                width: parent.width
                                spacing: 10 * scaleFactor
                                leftPadding: 10 * scaleFactor

                                ListLabel {
                                    text: "Edit"
                                    onTriggered: {
                                        conditionMenu.close();
                                        editMenu.open();
                                    }
                                }

                                ListLabel {
                                    text: "Delete"
                                    onTriggered: {
                                        conditionMenu.close();
                                        toolController.removeConditionAt(index);
                                    }
                                }
                            }
                        }
                    }
                }

                Label {
                    anchors {
                        left: parent.left
                        leftMargin: 40 * scaleFactor
                    }
                    width: parent.width - anchors.leftMargin
                    elide: Text.ElideRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: description
                    verticalAlignment: Text.AlignVCenter
                    color: Material.foreground
                    font {
                        pixelSize: 10 * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                }
            }

            Menu {
                id: editMenu
                width: parent.width

                Column {
                    anchors.margins: 10 * scaleFactor
                    width: parent.width
                    spacing: 10 * scaleFactor
                    leftPadding: 10 * scaleFactor

                    ComboBox {
                        id: editLevelBox
                        width: parent.width * 0.9
                        font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        textRole: "display"
                        model: toolController.levelNames

                        currentIndex: level - 1
                    }

                    TextField {
                        id: editConditionName
                        color: Material.accent
                        font.pixelSize: 14 * scaleFactor
                        width: parent.width * 0.9
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        text: conditionName
                    }

                    Row {
                        spacing: 10 * scaleFactor

                        OverlayButton {
                            id: keepEditsButton
                            iconUrl: DsaResources.iconComplete

                            onClicked: {
                                editMenu.close();

                                if (conditionName !== editConditionName.text)
                                    conditionName = editConditionName.text;

                                if (level !== editLevelBox.currentIndex + 1)
                                    level = editLevelBox.currentIndex + 1;
                            }
                        }

                        OverlayButton {
                            id: cancelEditsButton
                            iconUrl: DsaResources.iconClose

                            onClicked: {
                                editMenu.close();
                            }
                        }
                    }
                }
            }
        }
    }

    Menu {
        id: editMobileMenu
        width: parent.width
        y: parent.height - height

        Column {
            anchors.margins: 10 * scaleFactor
            width: parent.width
            spacing: 10 * scaleFactor
            leftPadding: 10 * scaleFactor

            ComboBox {
                id: editLevelMobileBox
                width: parent.width * 0.9
                font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                textRole: "display"
                model: toolController.levelNames

                currentIndex: conditionsList.currentLevel - 1
            }

            TextField {
                id: editConditionMobileName
                color: Material.accent
                font.pixelSize: 14 * scaleFactor
                width: parent.width * 0.9
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: conditionsList.currentName
            }

            Row {
                spacing: 10 * scaleFactor

                OverlayButton {
                    iconUrl: DsaResources.iconComplete

                    onClicked: {
                        if (conditionsList.currentName !== editConditionMobileName.text)
                            toolController.updateConditionName(conditionsList.currentIndex, editConditionMobileName.text);

                        if (conditionsList.currentLevel !== editLevelMobileBox.currentIndex + 1)
                            toolController.updateConditionLevel(conditionsList.currentIndex, editLevelMobileBox.currentIndex + 1);

                        editMobileMenu.close();
                        conditionsList.currentIndex = -1;
                    }
                }

                OverlayButton {
                    iconUrl: DsaResources.iconClose

                    onClicked: {
                        editMobileMenu.close();
                        conditionsList.currentIndex = -1;
                    }
                }
            }
        }
    }

    Button {
        id: createNewConditionButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 8 * scaleFactor
        }
        text: "Create new"
        font {
            pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
            bold: checked
        }
        width: 96 * scaleFactor
        background: Rectangle {
            color: Material.accent
            border.color: Material.foreground
        }

        onClicked: {
            createNewWizard.visible = true;
        }
    }

    Rectangle {
        id: mobileMenu
        property bool isOpen: y === manageAlertsRoot.y + manageAlertsRoot.height - height
        property int closedY: manageAlertsRoot.y + manageAlertsRoot.height
        property int openY: manageAlertsRoot.y + manageAlertsRoot.height - height - anchors.margins
        anchors {
            left: parent.left
            right: parent.right
            margins: 5 * scaleFactor
        }
        color: "transparent"
        height: manageAlertsRoot.height
        y: closedY

        MouseArea {
            anchors.fill: parent
            onClicked: mobileMenu.close()
        }

        Rectangle {
            anchors {
                fill: mobileActionColumn
                margins: -10 * scaleFactor
            }
            color: Material.background
            radius: 10 * scaleFactor
            border {
                color: Material.primary
                width: 1 * scaleFactor
            }
        }

        function open() {
            if (y === openY)
                return;

            conditionsList.highlightFollowsCurrentItem = true;
            animateVertical.from = closedY;
            animateVertical.to = openY;
            animateVertical.start();
        }

        function close(reset) {
            if (y === closedY)
                return;

            conditionsList.highlightFollowsCurrentItem = false;
            animateVertical.from = openY;
            animateVertical.to = closedY;
            animateVertical.start();
            if (reset)
              conditionsList.currentIndex = -1;
        }

        NumberAnimation {
            id: animateVertical
            target: mobileMenu
            properties: "y"
            duration: 250
            easing.type: Easing.OutQuad
        }

        Column {
            id: mobileActionColumn
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 10 * scaleFactor
            }

            spacing: 5 * scaleFactor

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Edit"
                onTriggered: {
                    editMobileMenu.open();
                    mobileMenu.close(false);
                }
            }

            ListSeparator{}

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Delete"
                onTriggered: {
                    toolController.removeConditionAt(conditionsList.currentIndex);
                    mobileMenu.close(true);
                }
            }
        }
    }

    Text {
        anchors {
            left: parent.left
            right: parent.right
            top: conditionsListTitle.bottom
            margins: 15 * scaleFactor
        }
        visible: conditionsList.count === 0
        text: "No Conditions.\n\nSelect 'Create New' to set up alert queries."
        color: Material.foreground
        horizontalAlignment: Text.AlignHCenter
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
    }

    AlertConditionsWizard {
        id: createNewWizard
        visible:  false
        anchors.fill: parent
    }
}
