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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Esri.ArcGISRuntime.OpenSourceApps.DSA

DsaPanel {
    id: messageFeedsRoot
    width: 350 * scaleFactor
    title: panelState === panelStateFeeds ? qsTr("Message Feeds")
                                           : (panelState === panelStateTrackDisplay ? qsTr("Track Display") : qsTr("Find Track"))
    iconSource: panelState === panelStateFeeds ? DsaResources.iconClose : ""
    leftActionIconSource: panelState === panelStateFeeds ? "" : DsaResources.iconBack
    titleActionClosesPanel: panelState === panelStateFeeds

    property alias controller: toolController
    property bool isMobile
    property real spinBoxHeight: DsaStyles.titleFontPixelSize * 1.6
    property real fontPixelSize: DsaStyles.secondaryTitleFontPixelSize * scaleFactor
    property real detailLabelFontPixelSize: 10 * scaleFactor
    property real trackControlSpacing: 4 * scaleFactor
    property real trackSectionSpacing: 18 * scaleFactor
    property real trackDividerSpacing: 20 * scaleFactor
    property real colorSwatchSpacing: 4 * scaleFactor
    property real colorSwatchSize: Math.min(32 * scaleFactor, (messageFeedsRoot.width - (16 * scaleFactor) - ((DsaResources.TrackDisplayColors.length - 1) * colorSwatchSpacing)) / DsaResources.TrackDisplayColors.length)
    readonly property int panelStateFeeds: 0
    readonly property int panelStateTrackDisplay: 1
    readonly property int panelStateFind: 2
    property int panelState: panelStateFeeds

    // Create the controller
    MessageFeedsController {
        id: toolController
    }

    onVisibleChanged: {
        // always switch back to the feeds page if the panel is hidden
        if (!visible)
            panelState = panelStateFeeds;
    }

    onTitleActionTriggered: panelState = panelStateFeeds

    StackLayout {
        currentIndex: panelState

        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }

        ListView {
            id: messageFeedsList
            clip: true
            model: toolController.messageFeeds
            highlightFollowsCurrentItem: isMobile
            highlight: Rectangle {
                radius: 5 * scaleFactor
                color: Material.accent
                opacity: 0.5
            }
            delegate:  ListItemDelegate {
                id: control
                width: parent.width
                height: 40 * scaleFactor
                itemSpacing: 1 * scaleFactor
                mainText: feedName
                itemChecked: feedVisible
                highlighted: toolController.selectedFeedIndex === index
                clickTogglesCheck: false
                rowTapExcludesCheckBox: true
                onRowTapped: {
                    toolController.selectedFeedIndex = index;
                }
                onRowPressAndHold: {
                    toolController.selectedFeedIndex = index;
                }

                onItemCheckedChanged: {
                    if (feedVisible === itemChecked)
                        return;

                    feedVisible = itemChecked
                }
                imageUrl: thumbnailUrl
                imageVisible: true
                imageFrameVisible: false

                Image {
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
                            toolController.selectedFeedIndex = index;
                            messageFeedsList.currentIndex = index;
                            if (!isMobile) {
                                feedMenu.open();
                            } else {
                                if (mobileMenu.isOpen) {
                                    mobileMenu.close();
                                } else {
                                    mobileMenu.open();
                                }
                            }
                        }
                    }

                    // Menu for Vehicle
                    Menu {
                        id: feedMenu
                        width: 150 * scaleFactor

                        Column {
                            anchors.margins: 10 * scaleFactor
                            width: parent.width
                            spacing: 10 * scaleFactor

                            ListLabel {
                                text: qsTr("Track Display")
                                onTriggered: {
                                    feedMenu.close();
                                    panelState = panelStateTrackDisplay;
                                }
                            }

                            ListLabel {
                                text: qsTr("Find Track")
                                separatorVisible: false
                                onTriggered: {
                                    feedMenu.close();
                                    panelState = panelStateFind;
                                }
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 32 * scaleFactor
                Layout.bottomMargin: trackControlSpacing

                Rectangle {
                    anchors.fill: parent
                    radius: 5 * scaleFactor
                    color: Material.accent
                    opacity: 0.5
                }

                Label {
                    anchors {
                        fill: parent
                        margins: 6 * scaleFactor
                    }
                    text: toolController.selectedFeedName
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font.pixelSize: fontPixelSize
                    font.italic: true
                }
            }

            // OBSERVATIONS
            CheckBox {
                id: switchObservations
                Layout.topMargin: trackSectionSpacing
                checked: toolController.selectedFeed.showPreviousObservations
                onCheckedChanged: toolController.selectedFeed.showPreviousObservations = checked
                text: "Show observations"
                font.pixelSize: fontPixelSize
            }
            GridLayout {
                visible: switchObservations.checked
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: trackControlSpacing
                rows: 4
                rowSpacing: trackControlSpacing
                SpinBox {
                    id: spinObservationsSize
                    Layout.row: 0
                    Layout.alignment: Qt.AlignHCenter
                    from: 1
                    to: 25
                    Layout.preferredHeight: spinBoxHeight
                    Layout.preferredWidth: drawer.width / 2.0
                    value: toolController.selectedFeed.sizeObservations
                    onValueChanged: {
                        if (!toolController.selectedFeed)
                            return;

                        toolController.selectedFeed.sizeObservations = value
                    }
                }
                Label {
                    Layout.row: 1
                    Layout.alignment: Qt.AlignCenter
                    text: "Marker size"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
                Row {
                    Layout.row: 2
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: trackControlSpacing * 2
                    spacing: colorSwatchSpacing

                    Repeater {
                        model: DsaResources.TrackDisplayColors

                        Rectangle {
                            width: colorSwatchSize
                            height: width
                            radius: width / 2
                            color: modelData
                            border {
                                color: Material.foreground
                                width: 1 * scaleFactor
                            }

                            Image {
                                anchors.centerIn: parent
                                height: parent.height
                                width: height
                                source: DsaResources.iconComplete
                                visible: toolController.selectedFeed && toolController.selectedFeed.colorObservations === modelData
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (toolController.selectedFeed)
                                        toolController.selectedFeed.colorObservations = modelData;
                                }
                            }
                        }
                    }
                }
                Label {
                    Layout.row: 3
                    Layout.alignment: Qt.AlignCenter
                    text: "Color"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
            }

            // TRACK LINE
            CheckBox {
                id: switchTrackLine
                Layout.topMargin: switchObservations.checked ? trackSectionSpacing : 0
                checked: toolController.selectedFeed.showTrackLine
                onCheckedChanged: toolController.selectedFeed.showTrackLine = checked
                text: "Show track lines"
                font.pixelSize: fontPixelSize
            }
            GridLayout {
                visible: switchTrackLine.checked
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: trackControlSpacing
                rows: 4
                rowSpacing: trackControlSpacing
                SpinBox {
                    id: spinTrackLineSize
                    Layout.row: 0
                    Layout.alignment: Qt.AlignHCenter
                    from: 1
                    to: 25
                    Layout.preferredHeight: spinBoxHeight
                    Layout.preferredWidth: drawer.width / 2.0
                    value: toolController.selectedFeed.sizeTrackLine
                    onValueChanged: {
                        if (!toolController.selectedFeed)
                            return;

                        toolController.selectedFeed.sizeTrackLine = value
                    }
                }
                Label {
                    Layout.row: 1
                    Layout.alignment: Qt.AlignCenter
                    text: "Line width"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
                Row {
                    Layout.row: 2
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: trackControlSpacing * 2
                    spacing: colorSwatchSpacing

                    Repeater {
                        model: DsaResources.TrackDisplayColors

                        Rectangle {
                            width: colorSwatchSize
                            height: width
                            radius: width / 2
                            color: modelData
                            border {
                                color: Material.foreground
                                width: 1 * scaleFactor
                            }

                            Image {
                                anchors.centerIn: parent
                                height: parent.height
                                width: height
                                source: DsaResources.iconComplete
                                visible: toolController.selectedFeed && toolController.selectedFeed.colorTrackLine === modelData
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (toolController.selectedFeed)
                                        toolController.selectedFeed.colorTrackLine = modelData;
                                }
                            }
                        }
                    }
                }
                Label {
                    Layout.row: 3
                    Layout.alignment: Qt.AlignCenter
                    text: "Color"
                    font.pixelSize: detailLabelFontPixelSize
                    font.italic: true
                }
            }

            // TRACK LENGTH
            ColumnLayout {
                visible: switchObservations.checked || switchTrackLine.checked
                Layout.fillWidth: true
                Layout.topMargin: trackDividerSpacing
                spacing: trackSectionSpacing

                Rectangle {
                    color: "gray"
                    Layout.fillWidth: true
                    radius: 5
                    height: 5
                }

                Label {
                    text: "Track Length"
                    font.pixelSize: fontPixelSize
                    Layout.alignment: Qt.AlignHCenter
                }

                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: trackControlSpacing

                    SpinBox {
                        id: spinObservations
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: spinBoxHeight
                        Layout.preferredWidth: drawer.width / 2.0
                        from: 0
                        to: 9999
                        editable: true
                        live: true
                        textFromValue: function(value) {
                            if (value < 1)
                                return "All"
                            else
                                return value
                        }

                        value: toolController.selectedFeed.maximumObservations
                        onValueChanged: toolController.selectedFeed.maximumObservations = value
                    }

                    Label {
                        text: "Number of observations"
                        font.pixelSize: detailLabelFontPixelSize
                        font.italic: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }

        ColumnLayout {

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 32 * scaleFactor

                Rectangle {
                    anchors.fill: parent
                    radius: 5 * scaleFactor
                    color: Material.accent
                    opacity: 0.5
                }

                Label {
                    anchors {
                        fill: parent
                        margins: 6 * scaleFactor
                    }
                    text: toolController.selectedFeedName
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font.pixelSize: fontPixelSize
                    font.italic: true
                }
            }

            TextField {
                id: textFindEntity
                Layout.fillWidth: true
                Layout.topMargin: trackControlSpacing
                placeholderText: qsTr("Search by track ID...")
                onTextChanged: toolController.findEntities(text);

                Button {
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                        margins: 2 * scaleFactor
                    }
                    visible: textFindEntity.text !== ""

                    width: height

                    background: Rectangle {
                        anchors.fill: parent
                        color: Material.primary
                    }

                    Image {
                        anchors.fill: parent
                        source: DsaResources.iconClose
                        fillMode: Image.PreserveAspectFit
                    }

                    onClicked: {
                        textFindEntity.text = "";
                        toolController.clearSearchResults();
                    }
                }
            }

            ListView {
                id: listEntityResults
                model: toolController.entityIdResults
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                delegate: ItemDelegate {
                    text: model.display
                    width: listEntityResults.width
                    rightPadding: resultActionsButton.width + 8 * scaleFactor

                    onClicked: {
                        toolController.selectEntity(index);
                        if (isMobile)
                            mapToolRow.reset();
                    }

                    Image {
                        id: resultActionsButton
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            rightMargin: 4 * scaleFactor
                        }
                        width: 32 * scaleFactor
                        height: width
                        rotation: 90
                        source: DsaResources.iconMenu
                        fillMode: Image.PreserveAspectFit

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (!isMobile) {
                                    resultActionsMenu.open();
                                } else if (mobileResultActionsMenu.isOpen) {
                                    mobileResultActionsMenu.close();
                                } else {
                                    listEntityResults.currentIndex = index;
                                    mobileResultActionsMenu.open();
                                }
                            }
                        }

                        Menu {
                            id: resultActionsMenu
                            width: 150 * scaleFactor

                            function selectAction(action) {
                                close();
                                toolController.selectEntityAction(index, action);
                            }

                            Column {
                                anchors.margins: 10 * scaleFactor
                                width: parent.width
                                spacing: 10 * scaleFactor

                                ListLabel {
                                    text: qsTr("Zoom to")
                                    onTriggered: resultActionsMenu.selectAction("Zoom to")
                                }

                                ListLabel {
                                    text: qsTr("Follow")
                                    onTriggered: resultActionsMenu.selectAction("Follow")
                                }

                                ListLabel {
                                    text: qsTr("Identify")
                                    onTriggered: resultActionsMenu.selectAction("Identify")
                                }

                                ListLabel {
                                    text: qsTr("Line of sight")
                                    separatorVisible: false
                                    onTriggered: resultActionsMenu.selectAction("Line of sight")
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: mobileResultActionsMenu
        visible: panelState === panelStateFind
        property bool isOpen: y === messageFeedsRoot.y + messageFeedsRoot.height - height
        property int closedY: messageFeedsRoot.y + messageFeedsRoot.height
        property int openY: messageFeedsRoot.y + messageFeedsRoot.height - height - anchors.margins
        anchors {
            left: parent.left
            right: parent.right
            margins: 5 * scaleFactor
        }
        color: "transparent"
        height: messageFeedsRoot.height
        y: closedY

        MouseArea {
            anchors.fill: parent
            onClicked: mobileResultActionsMenu.close()
        }

        Rectangle {
            anchors {
                fill: mobileResultActionsColumn
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

            mobileResultActionsAnimation.from = closedY;
            mobileResultActionsAnimation.to = openY;
            mobileResultActionsAnimation.start();
        }

        function close() {
            if (y === closedY)
                return;

            mobileResultActionsAnimation.from = openY;
            mobileResultActionsAnimation.to = closedY;
            mobileResultActionsAnimation.start();
        }

        function selectAction(action) {
            close();
            toolController.selectEntityAction(listEntityResults.currentIndex, action);
            mapToolRow.reset();
        }

        NumberAnimation {
            id: mobileResultActionsAnimation
            target: mobileResultActionsMenu
            properties: "y"
            duration: 250
            easing.type: Easing.OutQuad
        }

        Column {
            id: mobileResultActionsColumn
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 10 * scaleFactor
            }
            spacing: 5 * scaleFactor

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Zoom to")
                onTriggered: mobileResultActionsMenu.selectAction("Zoom to")
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Follow")
                onTriggered: mobileResultActionsMenu.selectAction("Follow")
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Identify")
                onTriggered: mobileResultActionsMenu.selectAction("Identify")
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Line of sight")
                onTriggered: mobileResultActionsMenu.selectAction("Line of sight")
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                separatorVisible: false
                text: qsTr("Cancel")
                onTriggered: mobileResultActionsMenu.close()
            }
        }
    }

    Rectangle {
        id: mobileMenu
        visible: panelState === panelStateFeeds
        property bool isOpen: y === messageFeedsRoot.y + messageFeedsRoot.height - height
        property int closedY: messageFeedsRoot.y + messageFeedsRoot.height
        property int openY: messageFeedsRoot.y + messageFeedsRoot.height - height - anchors.margins
        anchors {
            left: parent.left
            right: parent.right
            margins: 5 * scaleFactor
        }
        color: "transparent"
        height: messageFeedsRoot.height
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

            messageFeedsList.highlightFollowsCurrentItem = true;
            animateVertical.from = closedY;
            animateVertical.to = openY;
            animateVertical.start();
        }

        function close() {
            if (y === closedY)
                return;

            messageFeedsList.highlightFollowsCurrentItem = false;
            animateVertical.from = openY;
            animateVertical.to = closedY;
            animateVertical.start();
            messageFeedsList.currentIndex = -1;
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
                text: qsTr("Track Display")
                onTriggered: {
                    if (messageFeedsList.currentIndex > -1)
                        toolController.selectedFeedIndex = messageFeedsList.currentIndex;
                    mobileMenu.close();
                    panelState = panelStateTrackDisplay;
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Find Track")
                onTriggered: {
                    if (messageFeedsList.currentIndex > -1)
                        toolController.selectedFeedIndex = messageFeedsList.currentIndex;
                    mobileMenu.close();
                    panelState = panelStateFind;
                }
            }

            ListLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                separatorVisible: false
                text: qsTr("Cancel")
                onTriggered: {
                    mobileMenu.close();
                }
            }
        }
    }

    onPanelStateChanged: {
        if (panelState !== panelStateFeeds && mobileMenu.isOpen)
            mobileMenu.close();
    }
}
