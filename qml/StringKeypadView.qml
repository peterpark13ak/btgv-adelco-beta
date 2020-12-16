import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

ColumnLayout {
    id: stringKeypad
    width: parent.width
    spacing: 20

    property string title: "Type a string"
    property var target: null
    property string value: target.text
    property var validate: null
    property var disabledKeys: []

    signal save(string value)

    onValueChanged: {
        txtField.text = value
    }

    function applyKey(keyValue) {

        var str = txtField.text;
        var cursorPosition = (txtField.cursorPosition >= 0) ? txtField.cursorPosition : txtField.text.length;

        if (keyValue === "<-") {
            txtField.text = str.substring(0, cursorPosition - 1) + str.substring(cursorPosition);
            txtField.cursorPosition = cursorPosition > 0 ? cursorPosition - 1 : 0;
        } else {
            txtField.text = str.substring(0, cursorPosition) + keyValue + str.substring(cursorPosition);
            txtField.cursorPosition = cursorPosition + 1;
        }
    }

    MessageDialog {
        id: alertMessage
        title: "error message"
        icon: StandardIcon.Critical
    }

    Rectangle {
        width: 0; Layout.preferredHeight: 10
    }

    Text {
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        Layout.preferredHeight: 10
        Layout.preferredWidth: 400

        font.family: openSans.name
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter

        color: "#999999"
        text: stringKeypad.title
    }

    ColumnLayout {
        Layout.margins: 20

        // edit field row
        RowLayout {
            TextField {
                id: txtField
                Layout.preferredHeight: 50
                Layout.fillWidth: true
                font.family: openSans.name
                font.pixelSize: 16
                text: stringKeypad.value
            }

            Button {
                Layout.preferredHeight: 50
                Layout.alignment: Qt.AlignTop
                style: BlackButtonStyle {}
                text: "Clear"
                onClicked: txtField.text = "";
            }
        }

        // first keyboard row
        RowLayout {
            Repeater {
                model: [
                    {value: "1"}, {value: "2"}, {value: "3"}, {value: "4"}, {value: "5"},
                    {value: "6"}, {value: "7"}, {value: "8"}, {value: "9"}, {value: "0"},
                    {value: "-"}, {value: "$"}, {value: "<-"} ]

                delegate: KeyButton {
                    text: modelData.value
                    enabled: disabledKeys.indexOf(modelData.value) < 0
                    onClicked: applyKey(modelData.value)
                }
            }
        }

        // second keyboard row
        RowLayout {
            Repeater {
                model: [
                    {value: "Q", hsBefore: true }, {value: "W"}, {value: "E"}, {value: "R"}, {value: "T"},
                    {value: "Y"}, {value: "U"}, {value: "I"}, {value: "O"}, {value: "P"},
                    {value: "["}, {value: "]"} ]

                delegate: KeyButton {
                    text: modelData.value
                    enabled: disabledKeys.indexOf(modelData.value) < 0
                    Layout.leftMargin: modelData["hsBefore"] === true ? 25 : 0;
                    onClicked: applyKey(modelData.value)
                }
            }
        }

        // third keyboard row
        RowLayout {
            Repeater {
                model: [
                    {value: "A", fsBefore: true }, {value: "S"}, {value: "D"}, {value: "F"}, {value: "G"},
                    {value: "H"}, {value: "J"}, {value: "K"}, {value: "L"}, {value: ":"},
                    {value: "@"}, {value: "#"} ]

                delegate: KeyButton {
                    text: modelData.value
                    Layout.leftMargin: modelData["fsBefore"] === true ? 50 : 0;
                    enabled: disabledKeys.indexOf(modelData.value) < 0
                    onClicked: applyKey(modelData.value)
                }
            }
        }

        // fourth keyboard row
        RowLayout {
            Repeater {
                model: [
                    {value: "Z", fhsBefore: true }, {value: "X"}, {value: "C"}, {value: "V"}, {value: "B"},
                    {value: "N"}, {value: "M"}, {value: ","}, {value: "."} ]

                delegate: KeyButton {
                    text: modelData.value
                    Layout.leftMargin: modelData["fhsBefore"] === true ? 75 : 0;
                    enabled: disabledKeys.indexOf(modelData.value) < 0
                    onClicked: applyKey(modelData.value)
                }
            }
        }

        // fifth keyboard row
        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Button {
                Layout.preferredHeight: 50
                Layout.alignment: Qt.AlignTop
                style: BlackButtonStyle {}
                text: "Cancel"
                onClicked: stringKeypad.Stack.view.pop();
            }

            KeyButton {
                Layout.preferredWidth: 250
                text: "SPACE"
                enabled: disabledKeys.indexOf(" ") < 0
                onClicked: applyKey(" ")
            }

            Button {
                Layout.preferredHeight: 50
                Layout.alignment: Qt.AlignTop
                style: BlackButtonStyle {
                    fontColor: enabled === true ? "#e7e7e7" : "#606060"
                }
                text: "Ok"
                enabled: (txtField.text.trim().length > 0)
                onClicked: {
                    var result = txtField.text.trim();
                    if (validate !== null) {
                        var validateResult = validate(result);
                        if (validateResult.valid === false) {
                            alertMessage.text = validateResult.message;
                            alertMessage.open();
                            return;
                        }
                    }

                    if (target !== null) {
                        target.value = result;
                    }
                    stringKeypad.Stack.view.pop();
                    save(result);
                }
            }
        }
    }

    Rectangle {
        width: 0; Layout.fillHeight: true
    }
}
