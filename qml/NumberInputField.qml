import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

RowLayout {
    id: numberInputField

    spacing: _uimButtonHeight * 0.6

    property int labelWidth: 3 * _uimButtonHeight
    property string label
    property int value

    signal selected()

    onValueChanged: {
        if (value || 0 !== inputField.text) {
            inputField.text = value || 0;
        }
    }

    Text {
        visible: (label.length > 0)
        Layout.preferredWidth: labelWidth
        text: numberInputField.label
        color: "#999999"
        font.pixelSize: _uimFontSize
    }

    TextField {
        id: inputField
        Layout.preferredHeight: _uimButtonHeight * 0.6
        Layout.preferredWidth: 2 * _uimButtonHeight
        font.pixelSize: _uimFontSize

        inputMethodHints: Qt.ImhDigitsOnly

        onTextChanged: {
            numberInputField.value = text;
        }

        onFocusChanged: {
            if (focus === true) {
                // Qt.inputMethod.show();
                numberInputField.selected();
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                numberInputField.selected();
            }
        }
    }
}

