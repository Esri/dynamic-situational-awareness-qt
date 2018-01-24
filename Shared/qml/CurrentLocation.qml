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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import Esri.DSA 1.0


Item {
    width: locationText.width
    height: locationText.height

    Rectangle {
        anchors {
            margins: -2 * scaleFactor
            fill: locationText
        }
        color: Material.primary
        radius: 3 * scaleFactor
        opacity: 0.85
    }

    Column {
        id: locationText
        anchors.centerIn: parent
        spacing: 1 * scaleFactor
        Text {
            text: "N 42 18 1983 W 125 15 2114"
            font {
                pixelSize: 10 * scaleFactor
                family: DsaStyles.fontFamily
            }
            color: Material.foreground
        }
        Text {
            text: "Elevation: 123 z"
            font {
                pixelSize: 10 * scaleFactor
                family: DsaStyles.fontFamily
            }
            color: Material.foreground
        }
    }
}
