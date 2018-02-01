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
    Row {
        spacing: 20 * scaleFactor
        anchors.horizontalCenter: parent.horizontalCenter
        topPadding: 20 * scaleFactor

        CategoryIcon {
            id: homeIcon
            anchors.verticalCenter: parent.verticalCenter
            iconSource: DsaResources.iconHome
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
            iconSource: DsaResources.iconCreateReport
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
