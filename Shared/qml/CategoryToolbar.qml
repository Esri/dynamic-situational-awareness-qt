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

    Rectangle {
        anchors.fill: parent
        color: Material.primary
        opacity: 0.75
    }

    property string titleText

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
                titleText: "DSA - V"
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
                titleText: "Manage Map"
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
        },
        State {
            name: "options"
            PropertyChanges {
                target: optionsIcon
                selected: true
            }
            PropertyChanges {
                target: categoryToolbar
                titleText: "Settings"
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
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png"
            categoryName: "home"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: mapIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_mapview_dark_d.png"
            categoryName: "map"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: reportsIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_editmap_dark_d-2.png"
            categoryName: "reports"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: analysisIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_tooloverflow_dark_d.png"
            categoryName: "analysis"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: alertIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_help_dark_d.png"
            categoryName: "alerts"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: markupIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_edit_dark_d.png"
            categoryName: "markup"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: optionsIcon
            iconSource: "qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png"
            categoryName: "options"
            onCategorySelected: categoryToolbar.state = categoryName
        }
    }

}
