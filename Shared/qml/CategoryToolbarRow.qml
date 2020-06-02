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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime.OpenSourceApps.DSA 1.2
import QtQuick.Controls.Material 2.2

CategoryToolbar {
    Connections {
        target: appRoot
        onCloseDialogAccepted: Qt.quit()
    }

    Row {
        spacing: 20 * scaleFactor
        anchors.horizontalCenter: parent.horizontalCenter
        topPadding: 20 * scaleFactor

        CategoryIcon {
            id: homeIcon
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconDsaHome
            categoryName: "home"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: mapIcon
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconMap
            categoryName: "map"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: reportsIcon
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconReport
            categoryName: "reports"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: analysisIcon
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconTools
            categoryName: "analysis"
            onCategorySelected: categoryToolbar.state = categoryName
        }

        CategoryIcon {
            id: alertIcon
            anchors.verticalCenter: parent.verticalCenter
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
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconDraw
            categoryName: "markup"
            onCategorySelected: categoryToolbar.state = categoryName
        }
    }
}
