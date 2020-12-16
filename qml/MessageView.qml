import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: messageView

    property int closedHeight
    property int openHeight
    property int maxMessages: 100

    signal newMessage(string message)

    onHeightChanged: {
        if (state === "closed") {
            closedHeight = height;
        }
    }

    // visual properties
    // -------------------------------------------------------------
    border.color: "#666666"
    border.width: 1
    color: "#303030"
    clip: true

    state: "closed"

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: messageView; height: 0 }
        },
        State {
            name: "open"
            PropertyChanges { target: messageView; closedHeight: height; height: openHeight }
        },
        State {
            name: "closed"
            PropertyChanges { target: messageView; height: closedHeight }
        }
    ]

    transitions: Transition {
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
    }

    ListModel {
        id: messageModel
    }

    Component {
        id: messageDelegate
        Rectangle {
            height: _uimFontSize * 1.5
            width: parent.width
            color: "transparent"

            Text {
                text: timeStamp + " : " + message
                color: "#ffffff"
                font.pixelSize: _uimFontSize
                x: _uimFontSize
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    ListView {
        anchors.fill: parent
        spacing: _uimFontSize * 0.4

        model: messageModel
        delegate: messageDelegate

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: false

            onClicked: {
                messageView.state = (messageView.state === "open" ? "closed" : "open");
            }

            onReleased: {
                if (!propagateComposedEvents) {
                    propagateComposedEvents = true
                }
            }
        }
    }

    onNewMessage: {
        var d = new Date();
        var hr = ("00" + d.getHours()).slice(-2);
        var min = ("00" + d.getMinutes()).slice(-2);
        var sec = ("00" + d.getSeconds()).slice(-2);
        var mil = ("000" + d.getMilliseconds()).slice(-3);

        messageModel.insert(0, { timeStamp: hr+":"+min+":"+sec+"."+mil, message: message });

        if (messageModel.count > maxMessages) {
            messageModel.remove(0, 1);
        }
    }
}
