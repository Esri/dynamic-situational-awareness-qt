
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

Menu {
    id: conditionsWizardRoot
    property AlertConditionsController controller

    property bool readyToAdd: false

    Text {
        id: titleRow
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: DsaStyles.titleFontPixelSize * scaleFactor
        color: Material.accent
        text: "Create new Alert Condition"
    }

    SwipeView {
        id: pagesView
        anchors {
            top: titleRow.bottom
            bottom: createButton.top
            left: parent.left
            right: parent.right
            margins: 8 * scaleFactor
        }
    }

    OverlayButton {
        id: createButton
        anchors {
            bottom: parent.bottom
            right: parent.horizontalCenter
            margins: 8 * scaleFactor
        }
        iconUrl: DsaResources.iconComplete
        name: "Cancel"

        onClicked: {
            conditionsWizardRoot.close();
        }
    }

    OverlayButton {
        id: cancelButton
        anchors {
            bottom: parent.bottom
            left: parent.horizontalCenter
            margins: 8 * scaleFactor
        }
        iconUrl: DsaResources.iconClose
        name: "Cancel"

        onClicked: {
            conditionsWizardRoot.close();
        }
    }


}
