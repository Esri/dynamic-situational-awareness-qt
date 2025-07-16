import QtQuick
import QtQuick.Controls
import Esri.ArcGISRuntime.OpenSourceApps.DSA


Dialog {
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    visible: false
    x: appRoot.width / 2 - width / 2
    y: appRoot.height / 2 - height / 2

    property alias confirmationMessage: label.text
    property int buttonClicked: -1
    property var data

    Label {
        id: label
        font {
            pixelSize: 12 * scaleFactor
            family: DsaStyles.fontFamily
        }
        wrapMode: Text.Wrap
        width: parent.width
    }

    footer: DialogButtonBox {
        Button {
            text: "Yes"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: {
                buttonClicked = DialogButtonBox.Yes;
            }
        }
        Button {
            text: "No"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            onClicked: {
                buttonClicked = DialogButtonBox.No;
            }
        }
        Button {
            text: "Cancel"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            onClicked: {
                buttonClicked = DialogButtonBox.Cancel;
            }
        }
    }
}
