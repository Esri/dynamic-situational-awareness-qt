import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Esri.DSA 1.0
import Esri.ArcGISRuntime.Toolkit.Controls.CppApi 100.2

Row {
    id: reportToolRow
    spacing: 10 * scaleFactor
    visible: categoryToolbar.state === "home"
    onVisibleChanged: state = "clear"

    Image {
        id: optionsIcon
        source: DsaResources.iconSettings
        width: DsaStyles.primaryIconSize
        height: width

        MouseArea {
            anchors.fill: parent
            onClicked: menu.open()
        }

        Menu {
            id: menu
            MenuItem {
                text: "About"
                onTriggered: aboutTool.visible = true
            }
            MenuItem {
                text: "Settings"
                onTriggered: optionsTool.visible = true
            }
            MenuItem {
                text: "Close"
                onTriggered: {
                    closePrompt.visible = true;
                }
            }
        }
    }

    MessageDialog {
        id: closePrompt
        visible: false
        icon: StandardIcon.Question
        text: "Are you sure you want to close?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: Qt.quit();
        onNo:  visible = false;
    }
}


