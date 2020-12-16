import QtQuick 2.5
import QtQuick.Controls.Styles 1.4

TabViewStyle {
    property int tabWidth
    property int tabHeight

    tabsAlignment: Qt.AlignVCenter
    tabOverlap: 0
    frame: Item { }
    tab: Item {
        implicitWidth: tabWidth || control.width/control.count;
        implicitHeight: tabHeight || 50

        BorderImage {
            anchors.fill: parent
            border.bottom: 8
            border.top: 8
            source: styleData.selected ? "../images/tab_selected.png":"../images/tabs_standard.png"
            Text {
                anchors.centerIn: parent
                color: styleData.enabled ? "white" : "#666"
                text: styleData.title.toUpperCase()
                font.pixelSize: parent.height * 0.25
            }
            Rectangle {
                visible: index > 0
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 10
                width:1
                color: "#3a3a3a"
            }
        }
    }
}
