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
import QtQuick.Layouts
import QtQuick.Controls
import Esri.ArcGISRuntime.OpenSourceApps.DSA
import QtQuick.Controls.Material

CategoryToolbar {

    Connections {
        target: appRoot
        function onCloseDialogAccepted() {
            Qt.quit()
        }
    }

    Column {
        spacing: 10 * scaleFactor
        anchors {
            fill: parent
            topMargin: 10 * scaleFactor
        }

        CategoryIcon {
            id: homeIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: CalciteIcons.Calcite_home
            categoryName: "home"
            tooltipText: "Home"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: mapIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: CalciteIcons.Calcite_map
            categoryName: "map"
            tooltipText: "Map"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: reportsIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: CalciteIcons.Calcite_file_text
            categoryName: "reports"
            tooltipText: "Reports"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: analysisIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: CalciteIcons.Calcite_hammer
            categoryName: "analysis"
            tooltipText: "Analysis"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: alertIcon
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: CalciteIcons.Calcite_exclamation_mark_triangle
            categoryName: "alerts"
            tooltipText: "Alerts"
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
            iconSource: CalciteIcons.Calcite_pencil
            categoryName: "markup"
            tooltipText: "Markup"
            onCategorySelected: categoryToolbar.state = categoryName
        }
    }

    CategoryIcon {
        id: optionsIcon
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        iconSource: CalciteIcons.Calcite_gear
        categoryName: "options"
        tooltipText: "Options"
        onCategorySelected: menu.open()

        Menu {
            id: menu
            MenuItem {
                text: "About"
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
                onTriggered: aboutClicked()
            }
            MenuItem {
                text: "Settings"
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
                onTriggered: settingsClicked()
            }
            MenuItem {
                text: "Close"
                font {
                    pixelSize: 12 * scaleFactor
                    family: DsaStyles.fontFamily
                }
                onTriggered: appRoot.showCloseDialog("Are you sure you want to close?");
            }
        }
    }
}
