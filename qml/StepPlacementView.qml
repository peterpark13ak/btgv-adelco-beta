import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

ColumnLayout {

    // track ball -------------------------------------------------
    Rectangle {
        id: trackBall

        opacity: roiEdit ? 1 : 0.5
        enabled: roiEdit

        Layout.alignment: Qt.AlignTop
        Layout.topMargin: _uimButtonHeight * 0.2

        width: _uimHeight / 2
        height: width
        radius: width /2
        anchors.horizontalCenter: parent.horizontalCenter

        border.width: 0.5
        border.color: "#666"

        smooth: true
        color: "#111"

        Rectangle {
            id: trackBallArea

            width: Math.min(_uimHeight / 2, parent.width)
            height: width

            //anchors.fill: parent

            anchors.margins: 2
            anchors.horizontalCenter: parent.horizontalCenter
            color: "black"
            clip: true
            radius: width /2

            RadialGradient {
                source: trackBallArea
                anchors.fill: parent
                anchors.margins: 4
                horizontalOffset: - _uimButtonHeight * 0.4
                verticalOffset: - _uimButtonHeight * 0.4
                gradient: Gradient {
                    GradientStop { color: "#555555"; position: 0   }
                    GradientStop { color: "#161616"; position: 0.5 }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            drag.target: rectTarget
            drag.axis: Drag.XAndYAxis
        }

        Button {
            id: moveUp
            anchors.verticalCenter: trackBallArea.top
            anchors.horizontalCenter: trackBallArea.horizontalCenter
            anchors.verticalCenterOffset: _uimButtonHeight * 0.4
            iconSource: "../images/up.png"
            width: _uimButtonHeight * 1.2
            height: _uimButtonHeight * 1.2

            style: ButtonStyle { background: Rectangle {
                    color: "#20000000"
                    radius: control.width
                } }
            opacity: pressed ? 0.5 : 1

            onClicked: {
                rectTarget.y -= 1;
            }
        }

        Button {
            id: moveDown
            anchors.verticalCenter: trackBallArea.bottom
            anchors.horizontalCenter: trackBallArea.horizontalCenter
            anchors.verticalCenterOffset: - _uimButtonHeight * 0.2
            iconSource: "../images/down.png"
            width: _uimButtonHeight * 1.2
            height: _uimButtonHeight * 1.2

            style: ButtonStyle { background: Rectangle {
                    color: "#20000000"
                    radius: control.width
                } }
            opacity: pressed ? 0.5 : 1

            onClicked: {
                rectTarget.y += 1;
            }
        }

        Button {
            id: moveRight
            anchors.verticalCenter: trackBallArea.verticalCenter
            anchors.horizontalCenter: trackBallArea.right
            anchors.horizontalCenterOffset: - _uimButtonHeight * 0.2
            iconSource: "../images/next.png"
            width: _uimButtonHeight * 1.2
            height: _uimButtonHeight * 1.2

            style: ButtonStyle { background: Rectangle {
                    color: "#20000000"
                    radius: control.width
                } }
            opacity: pressed ? 0.5 : 1

            onClicked: {
                rectTarget.x += 1;
            }
        }

        Button {
            id: moveLeft
            anchors.verticalCenter: trackBallArea.verticalCenter
            anchors.horizontalCenter: trackBallArea.left
            anchors.horizontalCenterOffset: _uimButtonHeight * 0.2
            iconSource: "../images/previous.png"
            width: _uimButtonHeight * 1.2
            height: _uimButtonHeight * 1.2

            style: ButtonStyle { background: Rectangle {
                    color: "#20000000"
                    radius: control.width
                } }
            opacity: pressed ? 0.5 : 1

            onClicked: {
                rectTarget.x -= 1;
            }
        }

    }

    // action buttons ----------------------------------------------
    ExclusiveGroup {
        id: editMode

        onCurrentChanged: {
            positionModeActive = positionMode.checked;
        }
    }

    Text {
        text: "Children edit:"
        color: "#999999"
        font.pixelSize: 14
        visible: stepModel.editGroup || false;
    }

    RowLayout {
        id: modeButtons

        Layout.alignment: Qt.AlignTop
        Layout.fillWidth: true
        Layout.preferredHeight: roiEdit ? _uimButtonHeight * 0.8 : 0
        spacing: 0

        Switch {
            id: groupEditSwitch

            Layout.alignment: Qt.AlignTop
            style: BlackSwitchStyle {}
            visible: stepModel.editGroup || false;

            function refreshCheck() {
                checked = groupEditActive;
            }

            // for some unknown reason the checked property does not bind
            // with the groupEditActive, so we force binding
            Component.onCompleted: {
                editPanel.onGroupEditActiveChanged.connect(refreshCheck);
            }

            checked: groupEditActive
            onCheckedChanged: {
                groupEditActive = groupEditSwitch.checked;

                // force selection of the position mode
                positionMode.checked = true;
            }
        }

        Button {
            id: positionMode
            enabled: roiEdit

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: _uimButtonHeight * 0.8
            Layout.preferredWidth: parent.width / 2
            Layout.fillWidth: true
            style: BlackButtonStyle {
                fontSize: _uimFontSize
                pressColor: "#060606"
                background: BlackButtonBackground {
                    pressed: control.pressed || (control.checked || false)
                }
            }
            text: "Position"
            checkable: true
            exclusiveGroup: editMode
            checked: positionModeActive
        }

        Button {
            id: sizeMode
            enabled: roiEdit

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: _uimButtonHeight * 0.8
            Layout.preferredWidth: parent.width / 2
            Layout.fillWidth: true
            style: BlackButtonStyle {
                fontSize: _uimFontSize
                pressColor: "#060606"
                background: BlackButtonBackground {
                    pressed: control.pressed || (control.checked || false)
                }
            }
            text: "Size"
            checkable: true
            exclusiveGroup: editMode
            checked: ! positionModeActive
        }

    }

    // group edit actions
    // ---------------------------------------------------------------------------------------
    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: _uimButtonHeight
        visible: stepModel.editGroup || false;

        Button {
            id: next

            Layout.fillWidth: true
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle { releaseColor: "#369" }
            iconSource: "../images/next.png"
            opacity: enabled ? 1 : 0.5

            enabled: groupEditActive
            onClicked: {
                imageInspect.nextChild();
            }
        }
        Button {
            id: aligntop

            Layout.fillWidth: true
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle { releaseColor: "#369" }
            iconSource: "../images/aligntop.png"
            opacity: enabled ? 1 : 0.5

            enabled: groupEditActive
            onClicked: {
                imageInspect.alignTop();
            }
        }
        Button {
            id: samesize

            Layout.fillWidth: true
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle { releaseColor: "#369" }
            iconSource: "../images/samesize.png"
            opacity: enabled ? 1 : 0.5

            enabled: groupEditActive
            onClicked: {
                imageInspect.sameSize();
            }
        }
        Button {
            id: distribute

            Layout.fillWidth: true
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle { releaseColor: "#369" }
            iconSource: "../images/distribute.png"
            opacity: enabled ? 1 : 0.5

            enabled: groupEditActive
            onClicked: {
                imageInspect.distribute();
            }
        }
    }
}
