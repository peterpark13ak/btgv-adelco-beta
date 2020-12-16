import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    id: listPanel

    // properties
    // -------------------------------------------------------------------------
    property var stepsModel: []

    // signals
    // -------------------------------------------------------------------------
    signal stepSelected(int index, var step)

    // event handling
    // -------------------------------------------------------------------------
    function setActiveIndex(index) {
        list.currentIndex = index;
    }

    function refresh() {
        list.forceLayout();
    }

    // rendering
    // -------------------------------------------------------------------------
    Component {
        id: symbolDelegate

        Rectangle {
            height: _uimButtonHeight
            width: parent.width
            color: (modelData.trainable && ! modelData.trained) ? "#660000" : "#336699"

            Text {
                text: (modelData.level > stepsModel[0].level ? "| " : "") + modelData.name
                color: "#ffffff"
                font.pixelSize: _uimFontSize
                x: _uimButtonHeight * 0.4 + (modelData.level - stepsModel[0].level) * _uimButtonHeight * 0.2
                anchors.verticalCenter: parent.verticalCenter
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    stepSelected(modelData.index, modelData);
                }
            }
        }
    }

    ListView {
        id: list
        clip: true
        Layout.fillHeight: true
        Layout.fillWidth: true

        anchors.fill: parent

        highlightFollowsCurrentItem: true
        highlightMoveVelocity: 1000
        spacing: 5
        model: stepsModel
        delegate: symbolDelegate
    }

    Button {
        id: pageUp
        anchors.top: listPanel.top
        anchors.right: listPanel.right
        iconSource: "../images/up.png"
        width: _uimButtonHeight * 1.2
        height: _uimButtonHeight * 1.2

        style: ButtonStyle { background: Rectangle {
                color: "#80000000"
                radius: control.width
            } }
        opacity: pressed ? 0.5 : 1

        onClicked: {
            var pageSize = (list.height / (list.contentItem.children[0].height + list.spacing) + 0.5) | 0;
            var curIndex = list.currentIndex;
            var curPage = ((curIndex / pageSize) | 0) + 1;
            var pages = (stepsModel.length / pageSize + 0.5) | 0;

            if (curPage > 1) {
                var newIndex = (curPage - 2) * pageSize;
                list.currentIndex = newIndex;
            }
            else {
                list.currentIndex = 0;
            }
        }
    }

    Button {
        id: pageDown
        anchors.bottom: listPanel.bottom
        anchors.right: listPanel.right
        iconSource: "../images/down.png"
        width: _uimButtonHeight * 1.2
        height: _uimButtonHeight * 1.2

        style: ButtonStyle { background: Rectangle {
                color: "#80000000"
                radius: control.width
            } }
        opacity: pressed ? 0.5 : 1

        onClicked: {
            var pageSize = (list.height / (list.contentItem.children[0].height + list.spacing) + 0.5) | 0;
            var curIndex = list.currentIndex;
            var curPage = ((curIndex / pageSize) | 0) + 1;
            var pages = (stepsModel.length / pageSize + 0.5) | 0;

            if (curPage < pages) {
                var newIndex = (curPage + 1) * pageSize - 1;
                list.currentIndex = Math.min(newIndex, stepsModel.length - 1);
            }
            else {
                list.currentIndex = stepsModel.length - 1;
            }
        }
    }
}
