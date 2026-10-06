import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Button {
    id: primaryActionButton

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property real backgroundImplicitWidth: 0
    property real backgroundImplicitHeight: 0
    font.family: DsaStyles.fontFamily
    font.pixelSize: DsaStyles.toolFontPixelSize * scaleFactor * 1.5

    background: Rectangle {
        implicitWidth: primaryActionButton.backgroundImplicitWidth
        implicitHeight: primaryActionButton.backgroundImplicitHeight
        color: Material.accent
        opacity: !primaryActionButton.enabled ? 0.3 :
                 primaryActionButton.down ? 0.8 :
                 primaryActionButton.hovered ? 0.9 : 1.0
        border.color: Material.foreground
        border.width: 1 * primaryActionButton.scaleFactor
        radius: 2 * primaryActionButton.scaleFactor
    }
}