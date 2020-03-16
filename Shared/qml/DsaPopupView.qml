import QtQuick 2.12
import QtQuick.Controls 2.12

import "qrc:///Esri/ArcGISRuntime/Toolkit" as Toolkit

Toolkit.PopupStackView {
    id: identifyResults
    state: "slideOut"

    contentItem.data: Button {
       text: "Close"
       onClicked: identifyResults.state = "slideOut";
    }

    onVisibleChanged: {
        if (visible)
            identifyResults.state = "slideIn";
        else
            identifyResults.state = "slideOut";
    }

    states: [
        State {
            name: "slideIn";
            AnchorChanges {
                target: identifyResults;
                anchors.left: identifyResults.parent.left;
                anchors.right: identifyResults.parent.right;
            }
        },
        State {
            name: "slideOut";
            AnchorChanges {
                target: identifyResults;
                anchors.left: identifyResults.parent.right;
            }
        }
    ]

    transitions: [
        Transition {
            from: "slideOut";
            to: "slideIn";
            reversible: true;
            AnchorAnimation  {
                easing.type: Easing.InOutQuad
                duration: 400;
            }
            onRunningChanged: {
                if (!running && identifyResults.state === "slideOut")
                    identifyResults.visible = false;
            }
        }
    ]
}
