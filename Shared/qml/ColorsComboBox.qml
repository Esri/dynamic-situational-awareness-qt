import QtQuick
import QtQuick.Controls.Basic

ComboBox {
    id: comboBox
    model: ["red", "orange", "yellow", "green", "blue", "indigo", "violet", "black", "white", "gray"]
    delegate: ItemDelegate {
        Rectangle {
            anchors.fill: parent
            color: comboBox.model[index]
        }
    }

    contentItem: Text {
        text: ""
    }

    background: Rectangle {
        color: comboBox.model[comboBox.currentIndex]
    }
}
