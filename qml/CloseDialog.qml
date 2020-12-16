import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Rectangle {
    id: closeDialog

    visible: false
    anchors.fill: parent
    color: "#C0000000"

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: false
    }

    Rectangle {
        color: "#A0800000"

        width: closeDialog.height * 4 / 12
        height: parent.height * 5 / 12

        anchors.centerIn: parent

        Column {
            anchors.fill: parent
            anchors.margins: closeDialog.height / 24
            spacing: closeDialog.height / 24

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                height: closeDialog.height / 12

                text: "Close Application"
                style: BlackButtonStyle { bWidth: closeDialog.height * 3 / 12 }

                onClicked: {
                    Qt.quit();
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                height: closeDialog.height / 12

                text: "Shutdown"
                style: BlackButtonStyle { bWidth: closeDialog.height * 3 / 12 }

                onClicked: {
                    uiController.shutDown();
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                height: closeDialog.height / 12

                text: "Cancel"
                style: BlackButtonStyle { bWidth: closeDialog.height * 3 / 12 }

                onClicked: {
                    closeDialog.visible = false;
                }
            }
        }
    }
}
