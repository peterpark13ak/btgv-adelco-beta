import QtQuick 2.5

Item {
    id: formHeader

    width: parent.width

    property string text

    Column {
        width: parent.width

        Rectangle {
            width: parent.width
            height: _uimButtonHeight
            color: "transparent"

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: formHeader.text
                font.pixelSize: _uimH1FontSize
                color: "#999999"
            }
        }

        Rectangle {
            width: parent.width
            height: _uimButtonHeight / 10
            color: "#336699"
        }
    }
}

