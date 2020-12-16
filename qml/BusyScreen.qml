import QtQuick 2.5
import QtQuick.Controls 1.4

// Wait Cursor - Busy Indicator
// -------------------------------------------------------------
Rectangle {
	anchors.fill: parent
	z: 100
    color: "#88000000"
	visible: busyIndicator.running
    opacity: busyIndicator.running ? 1 : 0

    Behavior on opacity {
        OpacityAnimator { duration: 50 }
    }

    property bool busy: false

    signal cancel()

	MouseArea {
		anchors.fill: parent

        enabled: false;
        propagateComposedEvents: false;
        preventStealing: true;
        acceptedButtons: Qt.AllButtons

		onClicked: {
            console.log("Clicked");
			mouse.accepted = true;
		}
		onPressed: {
			mouse.accepted = true;
		}
		onReleased: {
			mouse.accepted = true;
		}
	}

	BusyIndicator {
        id: busyIndicator

        running: busy === true
        width: _uimButtonHeight
        height: _uimButtonHeight
		anchors.centerIn: parent

        onRunningChanged: {
            if (running === true) {
                cancelButton.visible = false;
                showCancelTimer.stop();
                showCancelTimer.start();
            }
        }
	}

    Timer {
        id: showCancelTimer
        interval: 10000
        onTriggered: {
            cancelButton.visible = true;
        }
    }

    Button {
        id: cancelButton

        style: BlackButtonStyle {
            pressColor: "#300"
            releaseColor: "#600"
        }

        height: _uimButtonHeight
        visible: false
        anchors.topMargin: _uimButtonHeight * 0.4
        anchors.top: busyIndicator.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Cancel"

        onClicked: {
            cancel();
        }
    }
}
