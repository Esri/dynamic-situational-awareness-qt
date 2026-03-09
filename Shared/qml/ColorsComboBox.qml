import QtQuick
import QtQuick.Controls.Material

ComboBox {
    id: control
    model: ["#e41a1c", "#377eb8", "#4daf4a", "#984ea3", "#ff7f00", "#ffff33", "#a65628", "#f781bf", "#999999"]
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
