import QtQuick 2.2
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4

Rectangle {
    id: indicatorView
    height: 80
    color: "transparent"

    property string indicatorColor: "#35e02f";
    property bool flashing: false;
    property string name
    property bool on: true

    Timer {
        id: flashTimer
        running: flashing && on
        repeat: true
        interval: 600
    }

    Column {
        id: indicatorLayout

        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 4

        StatusIndicator {
            id: indicator

            color: indicatorColor
            width: indicatorView.height * 0.75
            height: width
            anchors.horizontalCenter: parent.horizontalCenter

            on: indicatorView.on

            Connections {
                target: flashTimer
                onTriggered: if (flashing) indicator.active = !indicator.active
            }
        }

        Text {
            id: indicatorLabel
            text: name
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: indicatorView.height * 0.2 // Math.max(textSingleton.font.pixelSize, Math.min(32, mainWindow.toPixels(0.045)))
            styleColor: "#4e4e4e"
            color: "#999999"
            style: Text.Raised
        }
    }
}

