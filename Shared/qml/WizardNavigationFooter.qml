import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

RowLayout {
    id: footer

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property real bottomMargin: 0
    property bool backEnabled: false
    property bool nextEnabled: false
    property bool createEnabled: false

    signal backRequested()
    signal nextRequested()
    signal createRequested()
    signal cancelRequested()

    anchors {
        bottom: parent.bottom
        horizontalCenter: parent.horizontalCenter
        bottomMargin: footer.bottomMargin
    }

    Button {
        enabled: footer.backEnabled
        opacity: enabled ? 1.0 : 0.0
        Layout.margins: 4 * footer.scaleFactor
        Material.roundedScale: Material.NotRounded
        height: nextButton.height
        width: nextButton.width
        text: "Back"
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        font.pixelSize: DsaStyles.toolFontPixelSize

        onClicked: footer.backRequested()
    }

    ToolIcon {
        Layout.margins: {
            left: 0
            top: 4 * footer.scaleFactor
            right: 4 * footer.scaleFactor
            bottom: 4 * footer.scaleFactor
        }
        enabled: footer.createEnabled
        opacity: enabled ? 1.0 : 0.5
        iconSource: DsaResources.iconComplete
        toolName: "Create"
        labelColor: Material.accent
        onToolSelected: footer.createRequested()
    }

    ToolIcon {
        Layout.margins: {
            left: 0
            top: 4 * footer.scaleFactor
            right: 4 * footer.scaleFactor
            bottom: 4 * footer.scaleFactor
        }
        toolName: "Cancel"
        iconSource: DsaResources.iconClose
        onToolSelected: footer.cancelRequested()
    }

    Button {
        id: nextButton
        enabled: footer.nextEnabled
        opacity: enabled ? 1.0 : 0.0
        Layout.margins: {
            left: 0
            top: 4 * footer.scaleFactor
            right: 4 * footer.scaleFactor
            bottom: 4 * footer.scaleFactor
        }
        Material.roundedScale: Material.NotRounded
        height: 32 * footer.scaleFactor
        width: 64 * footer.scaleFactor
        text: "Next"
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        font.pixelSize: DsaStyles.toolFontPixelSize

        onClicked: footer.nextRequested()
    }
}