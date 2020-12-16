import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

ColumnLayout {
    id: booleanInput

    width: parent.width
    height: readOnly === true ? _uimButtonHeight * 0.2 : _uimButtonHeight

    spacing: _uimButtonHeight * 0.1

    property string code
    property string label
    property bool value
    property bool enabled

    property bool readOnly: false

    //signal selected()
    signal accepted()
    signal change(bool value)

    onValueChanged: {
        if ((value || false) !== inputField.checked) {
            inputField.checked = value || false;
            change(inputField.checked);
        }
    }

    RowLayout {
        spacing: _uimButtonHeight * 0.2

        Text {
            visible: (label.length > 0)
            Layout.preferredWidth: _uimButtonHeight * 3
            text: booleanInput.label
            color: "#999999"
            font.pixelSize: _uimFontSize
        }

        Text {
            visible: readOnly === true

            Layout.preferredWidth: _uimButtonHeight * 1.2
            font.pixelSize: _uimFontSize
            text: value

            color: "white"
        }
    }

    RowLayout {
        spacing: 2

        visible: readOnly === false

        Switch {
            id: inputField

            style: BlackSwitchStyle {}

            Layout.preferredHeight: _uimButtonHeight
            Layout.preferredWidth: _uimButtonHeight * 2
            visible: readOnly === false

            onCheckedChanged: {
                booleanInput.value = checked;
            }

        }

        Button {
            id: acceptbutton
            iconSource: "../images/check.png"
            style: BlackButtonStyle { }
            Layout.preferredHeight: _uimButtonHeight
            visible: readOnly === false
            enabled: parent.enabled

            onClicked: {
                accepted();
            }
        }
    }

}

