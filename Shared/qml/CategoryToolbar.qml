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
import Esri.DSA 1.0
import QtQuick.Controls.Material 2.2

Item {
    id: categoryToolbar

    property string appTitle
    property string titleText
    signal settingsClicked()
    signal aboutClicked()

    Rectangle {
        anchors.fill: parent
        color: Material.primary
        opacity: 0.75
    }


    state: "home"
    states: [
        State {
            name: "home"
            PropertyChanges {
                target: homeIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: appTitle
            }
        },
        State {
            name: "map"
            PropertyChanges {
                target: mapIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: "Map"
            }
        },
        State {
            name: "reports"
            PropertyChanges {
                target: reportsIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: "Create Report"
            }
        },
        State {
            name: "analysis"
            PropertyChanges {
                target: analysisIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: "Analysis"
            }
        },
        State {
            name: "alerts"
            PropertyChanges {
                target: alertIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: "Alerts"
            }
        },
        State {
            name: "markup"
            PropertyChanges {
                target: markupIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: "Markup"
            }
        }
    ]

    Column {
        anchors {
            fill: parent
            topMargin: 10 * scaleFactor
        }

        CategoryIcon {
            id: homeIcon
            iconSource: DsaResources.iconHome
            categoryName: "home"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: mapIcon
            iconSource: DsaResources.iconMap
            categoryName: "map"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: reportsIcon
            iconSource: DsaResources.iconCreateReport
            categoryName: "reports"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: analysisIcon
            iconSource: DsaResources.iconTools
            categoryName: "analysis"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: alertIcon
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
            iconSource: DsaResources.iconDraw
            categoryName: "markup"
            onCategorySelected: categoryToolbar.state = categoryName
        }
    }

    Column {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        CategoryIcon {
            id: optionsIcon

            iconSource: DsaResources.iconSettings
            categoryName: "options"
            onCategorySelected: menu.open()
        }

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
                onTriggered: Qt.quit()
            }
        }
    }
}
