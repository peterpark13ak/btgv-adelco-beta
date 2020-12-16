import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

SwitchStyle {

    groove: Rectangle {

        implicitHeight: _uimButtonHeight
        implicitWidth: 2 * _uimButtonHeight

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.width/2 - 2
            height: _uimButtonHeight / 2
            anchors.margins: 2
            color: control.checked ? "#468bb7" : "#222"
            Behavior on color {ColorAnimation {}}
            Text {
                font.pixelSize: _uimFontSize
                color: "white"
                anchors.centerIn: parent
                text: "ON"
            }
        }

        Item {
            width: parent.width/2
            height: parent.height
            anchors.right: parent.right
            Text {
                font.pixelSize: _uimFontSize
                color: "white"
                anchors.centerIn: parent
                text: "OFF"
            }
        }

        color: "#222"
        border.color: "#444"
        border.width: 2    
    }

    handle: Rectangle {
        width: parent.parent.width/2
        height: control.height
        color: "#444"
        border.color: "#555"
        border.width: 2
    }
}
