import QtQuick 2.7
import QtQuick.Dialogs 1.2

Item {
    id: root
    signal fileChosen(url url);

    property var supportedExtensions: []
    property alias folder: fileDialog.folder
    property alias fileUrl: fileDialog.fileUrl

    function open() {
        fileDialog.open();
    }

    DropArea {
        anchors.fill: parent

        onDropped: {
            if (!drop.hasText)
                return;

            if (drop.urls.length !== 1)
                return;

            if (!root.validateFileExtension(drop.urls[0]))
                return;

            if (drop.proposedAction !== Qt.MoveAction && drop.proposedAction !== Qt.CopyAction)
                return;

            drop.acceptProposedAction();
            root.fileChosen(drop.urls[0]);
        }
    }

    FileDialog {
        id: fileDialog

        onAccepted: {
            root.fileChosen(fileDialog.fileUrl)
        }
    }

    function validateFileExtension(filePath) {
        var extension = filePath.split('.').pop();
        var idx = supportedExtensions.indexOf(extension);

        return idx !== -1;
    }

    onSupportedExtensionsChanged: {
        var nameFiltersString = "Message files (";
        for (var i = 0; i < supportedExtensions.length; i++)
            nameFiltersString += "*." + supportedExtensions[i] + " ";

        nameFiltersString += ")";
        fileDialog.nameFilters = nameFiltersString;
    }
}
