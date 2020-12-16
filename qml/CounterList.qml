import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

import com.darebit.blisterocv 1.0

Rectangle {
    id: counterTable
    color: "transparent"

    property string prefix
    property bool isActive: false
    property bool localPlcDataToggle: statusPlcDataToggle
    property var rows: [
        { label: "camera triggers:" },
        { label: "passed prod #1:" },
        { label: "passed prod #2:" },
        { label: "passed prod #3:" },
        { label: "passed prod #4:" },
        { label: "failed prod #1:" },
        { label: "failed prod #2:" },
        { label: "failed prod #3:" },
        { label: "failed prod #4:" },
        { label: "no-answer fails:" }
    ]
    property double fontSize: 14

    onLocalPlcDataToggleChanged: {
        if (!isActive || counterFields.itemAt( 0) === null) {
            return;
        }
        counterFields.itemAt( 0).children[1].text = plcStatusData[prefix + "CameraTriggers"];
        counterFields.itemAt( 1).children[1].text = plcStatusData[prefix + "Passed01"];
        counterFields.itemAt( 2).children[1].text = plcStatusData[prefix + "Passed02"];
        counterFields.itemAt( 3).children[1].text = plcStatusData[prefix + "Passed03"];
        counterFields.itemAt( 4).children[1].text = plcStatusData[prefix + "Passed04"];
        counterFields.itemAt( 5).children[1].text = plcStatusData[prefix + "Fails01"];
        counterFields.itemAt( 6).children[1].text = plcStatusData[prefix + "Fails02"];
        counterFields.itemAt( 7).children[1].text = plcStatusData[prefix + "Fails03"];
        counterFields.itemAt( 8).children[1].text = plcStatusData[prefix + "Fails04"];
        counterFields.itemAt( 9).children[1].text = plcStatusData[prefix + "NoAnswerFails"];
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: _uimButtonHeight * 0.1

        Repeater {
            id: counterFields

            model: rows

            delegate: RowLayout {
                width: parent.width
                Layout.topMargin: counterTable.fontSize / 3

                Text {
                    Layout.preferredWidth: fontSize * 10
                    text: modelData.label
                    color: "#999999"
                    font.pixelSize: fontSize
                }
                Text {
                    Layout.fillWidth: true
                    Layout.rightMargin: 10
                    horizontalAlignment: Text.AlignRight
                    color: "#e0e0e0"
                    font.pixelSize: fontSize
                }
            }
        }

    }
}
