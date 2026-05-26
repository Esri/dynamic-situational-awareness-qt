import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Esri.ArcGISRuntime.OpenSourceApps.DSA

ComboBox {
    id: control
    model: DsaResources.TrackDisplayColors
    delegate: ItemDelegate {
        width: control.width
        Rectangle {
            anchors.fill: parent
            color: control.model[index]
        }
    }

    contentItem: Text {
        text: ""
    }

    background: Rectangle {
        radius: 5
        color: model[currentIndex]
    }
}
