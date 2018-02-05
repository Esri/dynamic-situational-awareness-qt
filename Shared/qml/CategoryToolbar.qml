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

Item {
    id: categoryToolbar

    property string appTitle
    property string titleText
    signal settingsClicked()
    signal aboutClicked()

    Rectangle {
        anchors.fill: parent
        color: Material.primary
        opacity: 0.9
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
}
