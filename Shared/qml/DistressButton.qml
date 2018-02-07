// Copyright 2018 ESRI
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
import Esri.DSA 1.0

OverlayButton {
    id: distressButton
    property var messageFeedsController

    iconUrl: messageFeedsController.locationBroadcastInDistress ?
                 DsaResources.iconDistressRed :
                 DsaResources.iconDistressGray

    onClicked: {
        messageFeedsController.locationBroadcastInDistress = !messageFeedsController.locationBroadcastInDistress;
    }
}
