import QtQuick 2.5
import QtQuick.Extras 1.4

DelayButton {
    property bool turnedOn: false;

    signal confirm(bool setOn)

    delay: 800
    text: turnedOn ? "Stop" : "Start";

    onConfirm: {
        checked = false;
        turnedOn = setOn;
        enabled = true;
    }
}
