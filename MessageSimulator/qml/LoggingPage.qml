import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.0
import QtQuick 2.7

Rectangle {
    id: loggingPage

    property alias  loggingText: loggingText.text

    Text {
        id: loggingText
        anchors.fill: loggingPage
        wrapMode: Text.Wrap
    }
}

