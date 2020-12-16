import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

ColumnLayout {
    id: numberInputSpinner

    width: parent.width
    height: readOnly === true ? _uimButtonHeight * 0.2 : _uimButtonHeight * 1.3

    spacing: _uimButtonHeight * 0.1

    property string code
    property string label
    property int value
    property var repeatTarget
    property bool repeatActive
    property int repeatSign
    property int repeatStep
    property int minValue
    property int maxValue
    property bool enabled

    property bool readOnly: false

    signal selected()
    signal accepted()
    signal change(int value)

    onValueChanged: {
        if ((value || 0) !== inputField.text) {

            if (value > maxValue) {
                value = maxValue;
            }

            if (value < minValue) {
                value = minValue;
            }

            inputField.text = value || 0;
            change(inputField.text);
        }
    }

    Timer {
        id: repeatTimer
        running: false
        repeat: true
        interval: 100
        onTriggered: {
            inputField.text = (inputField.text | 0) + repeatSign * repeatStep;
        }
        onRunningChanged: {
            if (running === false) {
                changeStep.stop();
            } else {
                changeStep.start();
            }
        }
    }

    Timer {
        id: changeStep
        running: false
        repeat: false
        interval: 3000
        onTriggered: {
            repeatStep = 100;
        }
    }

    Timer {
        id: repeatStartTimer
        running: false
        repeat: false
        interval: 1500
        onTriggered: {
            repeatActive = true;
            repeatStep = 1;
            repeatTimer.start();
        }
    }

    RowLayout {
        spacing: _uimButtonHeight * 0.2

        Text {
            visible: (label.length > 0)
            Layout.preferredWidth: _uimButtonHeight * 3
            text: numberInputSpinner.label
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

        TextField {
            id: inputField
            Layout.preferredHeight: _uimButtonHeight
            Layout.preferredWidth: _uimButtonHeight * 1.2
            font.pixelSize: _uimFontSize

            visible: readOnly === false
            text: "0"
            enabled: parent.enabled

            inputMethodHints: Qt.ImhDigitsOnly

            onTextChanged: {
                numberInputSpinner.value = text;
            }

            onFocusChanged: {
                if (focus === true) {
                    inputField.select();
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    numberInputSpinner.selected();
                }
            }
        }

        Button {
            id: decreasebutton
            iconSource: "../images/minus.png"
            style: BlackButtonStyle { }
            Layout.preferredHeight: _uimButtonHeight
            visible: readOnly === false
            enabled: parent.enabled

            onClicked: {
                if (repeatActive) return;
                inputField.text = (inputField.text | 0) - 1
            }
            onPressedChanged: {
                if (pressed === false) {
                    repeatStartTimer.stop();
                    repeatTimer.stop();
                } else {
                    repeatActive = false;
                    repeatTarget = decreasebutton;
                    repeatSign = -1;
                    repeatStartTimer.start();
                }
            }
        }

        Button {
            id: increasebutton
            iconSource: "../images/add.png"
            style: BlackButtonStyle { }
            Layout.preferredHeight: _uimButtonHeight
            visible: readOnly === false
            enabled: parent.enabled

            onClicked: {
                if (repeatActive) return;
                inputField.text = (inputField.text | 0) + 1
            }
            onPressedChanged: {
                if (pressed === false) {
                    repeatStartTimer.stop();
                    repeatTimer.stop();
                } else {
                    repeatActive = false;
                    repeatTarget = increasebutton;
                    repeatSign = 1;
                    repeatStartTimer.start();
                }
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

