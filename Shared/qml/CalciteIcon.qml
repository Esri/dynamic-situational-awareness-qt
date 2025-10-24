import QtQuick
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    property alias iconName: textId.text
    property alias color: textId.color
    property alias pixelSize: textId.font.pixelSize
    Text {
        anchors.centerIn: parent
        id: textId
        color: "white"
        font {
            family: appRoot.isMobile ? CalciteIcons.FontFamily24 : CalciteIcons.FontFamily32
            pixelSize: (appRoot.isMobile ? 24 : 32) * appRoot.scaleFactor
        }
    }
}
