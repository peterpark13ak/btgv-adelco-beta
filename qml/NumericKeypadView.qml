import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

ColumnLayout {
    id: numKeypad
    width: parent.width
    spacing: _uimButtonHeight * 0.4

    property string title: "Enter the value"
    property var target
    property int value: target.value

    onValueChanged: {
        numField.text = value
    }

    Rectangle {
        width: 0; Layout.preferredHeight: _uimButtonHeight * 0.2
    }

    Text {
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        Layout.preferredHeight: _uimButtonHeight * 0.2
        Layout.preferredWidth: 4 * _uimButtonHeight

        font.family: openSans.name
        font.pixelSize: _uimFontSize
        horizontalAlignment: Text.AlignHCenter

        color: "#999999"
        text: numKeypad.title
    }

    TextField {
        id: numField

        Layout.margins: _uimButtonHeight * 0.4
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        Layout.preferredHeight: _uimButtonHeight * 0.6
        Layout.preferredWidth:  _uimButtonHeight * 4

        font.family: openSans.name
        font.pixelSize: _uimFontSize

        text: numKeypad.value
    }

    GridLayout {
        id: numPad
        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

        columns: 3
        rows: 4
        rowSpacing: _uimButtonHeight * 0.2
        columnSpacing: _uimButtonHeight * 0.2

        Layout.preferredHeight: 5 * _uimButtonHeight

        Repeater {
            model: [ "7", "8", "9", "4", "5", "6", "1", "2", "3", "", "0", "" ]

            delegate: Button {
                id: numKey

                Layout.preferredWidth: _uimButtonHeight * 1.2
                Layout.preferredHeight: _uimButtonHeight * 1.2

                style: BlackButtonStyle {
                    bWidth: _uimButtonHeight * 1.2
                    fontFamily: adventPro.name
                    fontSize: _uimFontSize
                }

                text: modelData

                // create spacer for empty title
                opacity: modelData === "" ? 0 : 1;

                onClicked: {
                    var inputText = text === "." ? ".0" : text;
                    numField.text = +(numField.text + inputText) || 0.0; //itemSelected(modelData.code);
                }
            }
        }
    }

    RowLayout {
        Layout.preferredHeight: _uimButtonHeight
        Layout.margins: _uimButtonHeight * 0.4
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        spacing: _uimButtonHeight * 0.2

        Button {
            Layout.preferredHeight: _uimButtonHeight
            Layout.alignment: Qt.AlignTop
            style: BlackButtonStyle {}
            text: "Cancel"
            onClicked: numKeypad.Stack.view.pop();
        }

        Button {
            Layout.preferredHeight: _uimButtonHeight
            Layout.alignment: Qt.AlignTop
            style: BlackButtonStyle {}
            text: "Clear"
            onClicked: numField.text = 0;
        }

        Button {
            Layout.preferredHeight: _uimButtonHeight
            Layout.alignment: Qt.AlignTop
            style: BlackButtonStyle {}
            text: "Ok"
            onClicked: {
                target.value = numField.text;
                numKeypad.Stack.view.pop();
            }
        }
    }

    Rectangle {
        width: 0; Layout.fillHeight: true
    }

}
