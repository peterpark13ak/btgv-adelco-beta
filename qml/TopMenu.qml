import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

RowLayout {
    id: topmenu

    // property bool running;

    signal itemSelected(string menuItem)

    spacing: _uimButtonHeight * 0.2

    function enabledItem(code) {

        if (statusPlcConnected === false) {
            return false;
        }

        if (code === "MENU_BATCH_CONFIGURATION" || code === "MENU_SYSTEM_TEST" || code === "MENU_CLOSE") {
            return ! statusPlcRunning;
        }
        else {
            return true;
        }
    }

    Rectangle {
        id: logowrapper

        Layout.preferredHeight: parent.height
        Layout.preferredWidth: parent.height
        color: "transparent"

        Image {
            anchors.fill: parent
            source: "../images/logo.png"
            fillMode: Image.PreserveAspectFit
        }
    }

    Repeater {
        model: [
            {
                title: "Batch Configuration",
                code:   "MENU_BATCH_CONFIGURATION",
            },
            {
                title: "System Configuration",
                code:   "MENU_SYSTEM_CONFIGURATION",
            },
            {
                title: "",
                code:   "MENU_",
            },
            {
                title: "SystemTest",
                code:   "MENU_SYSTEM_TEST",
            },
            {
                title:  "Close",
                code:   "MENU_CLOSE",
            }
        ]

        delegate: Button {
            id: menuItem

            enabled: enabledItem(modelData.code)

            Layout.preferredHeight: parent.height;
            style: BlackButtonStyle {}

            text: modelData.title

            // create spacer for empty title
            opacity: modelData.title === "" ? 0 : 1;
            Layout.fillWidth: modelData.title === "" ? true : false;

            onClicked: {
                itemSelected(modelData.code);
            }
        }
    }
}
