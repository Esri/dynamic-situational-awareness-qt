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

import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import Esri.DSA 1.0
import QtQuick.Controls.Material 2.2

CategoryToolbar {
    Column {
        spacing: 2 * scaleFactor
        anchors {
            fill: parent
            topMargin: 10 * scaleFactor
        }

        CategoryIcon {
            id: homeIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: DsaResources.iconHome
            categoryName: "home"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: mapIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: DsaResources.iconMap
            categoryName: "map"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: reportsIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: DsaResources.iconCreateReport
            categoryName: "reports"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: analysisIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: DsaResources.iconTools
            categoryName: "analysis"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: alertIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: DsaResources.iconAlert
            categoryName: "alerts"
            onCategorySelected: categoryToolbar.state = categoryName

            ViewedAlerts {
                anchors {
                    right: parent.right
                    top: parent.top
                }
            }
        }

        CategoryIcon {
            id: markupIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: DsaResources.iconDraw
            categoryName: "markup"
            onCategorySelected: categoryToolbar.state = categoryName
        }
    }

    CategoryIcon {
        id: optionsIcon
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        iconSource: DsaResources.iconSettings
        categoryName: "options"
        onCategorySelected: menu.open()

        Menu {
            id: menu
            MenuItem {
                text: "About"
                onTriggered: aboutClicked()
            }
            MenuItem {
                text: "Settings"
                onTriggered: settingsClicked()
            }
            MenuItem {
                text: "Close"
                onTriggered: {
                    closePrompt.visible = true;
                }
            }
        }
    }

    MessageDialog {
        id: closePrompt
        visible: false
        icon: StandardIcon.Question
        title: appTitle
        text: "Are you sure you want to close?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: Qt.quit();
        onNo:  visible = false;
    }
}
