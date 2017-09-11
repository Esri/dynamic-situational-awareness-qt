
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
import QtQuick.Dialogs 1.2
import Esri.Vehicle 1.0

Item {
    id: locationItem

    LocationController {
        id: locationController
        enabled: enabledCheckBox.checked;
        simulated: simCheckBox.checked
    }

    FileDialog {
        id: dialog
        folder: locationController.defaultFileSearchPath
        onAccepted: {
            locationController.gpxFilePath = fileUrl;
        }
    }

    Rectangle {
        anchors.centerIn: parent

        width: parent.width / 4
        height: parent.height / 3;
        Column {
            Row {
                CheckBox {
                    id: enabledCheckBox
                    text: "Enabled"
                }
            }

            Row {
                CheckBox {
                    id: simCheckBox
                    text: "Simulated"
                }
            }

            Row {
                Button {
                    visible: simCheckBox.checked
                    text: "Choose Gpx file"
                    onReleased: {
                        dialog.open();
                    }
                }
            }
        }
    }

}
