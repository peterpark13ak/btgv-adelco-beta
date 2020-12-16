import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

ColumnLayout {
    id: resultsList

    // properties
    // -------------------------------------------------------------
    property var valueList: ({})

    // visual properties
    // -------------------------------------------------------------
    Layout.alignment: Qt.AlignTop
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.margins: 0

    // signals
    // -------------------------------------------------------------
    signal newStatistics(string newValues);

    // event handling
    // -------------------------------------------------------------
    Component.onCompleted: {
        uiController.newStatistics.connect(newStatistics);
    }

    onNewStatistics: {
        var values = JSON.parse(newValues);
        for (var i = 0; i < values.length; i++) {

            if (valueList.hasOwnProperty(values[i].id)) {
                var index = valueList[values[i].id];
                // set model index to new value
                resultsModel.set(index, {
                                     label: values[i].label,
                                     stat: "" + values[i].value,
                                     groupHeader: values[i].isGroupLabel
                                 });
            }
            else {
                // append to model and get the index - store the index
                resultsModel.append({
                                    label: values[i].label,
                                    stat: "" + values[i].value,
                                    groupHeader: values[i].isGroupLabel
                                });
                valueList[values[i].id] = resultsModel.count - 1;
            }
        }
    }

    // rendering
    // -------------------------------------------------------------
    FormHeader {
        Layout.alignment: Qt.AlignTop
        Layout.preferredHeight: 50
        Layout.fillWidth: true
        Layout.margins: 0

        text: "Inspection Results"
    }

    ListModel {
        id: resultsModel
    }

    Component {
        id: resultDelegate
        Rectangle {
            height: 35
            width: parent.width
            color: model.groupHeader === true ? "#333333" : "transparent"

            Row {
                anchors.fill: parent
                anchors.margins: 5

                Text {
                    width: Math.max(implicitWidth, 200)
                    text: label
                    color: model.groupHeader !== true ? "#336699" : "#ffffff"
                    font.bold: model.groupHeader
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    width: Math.max(implicitWidth + 10, 60)
                    horizontalAlignment: Text.AlignRight
                    text: stat
                    color: "#ffffff"
                    font.pixelSize: 14
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

        }
    }

    ListView {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 5

        model: resultsModel
        delegate: resultDelegate
    }

    // action buttons
    // -------------------------------------------------------------
    RowLayout {
        spacing: 10

        Button {
            Layout.preferredHeight: 50
            style: BlackButtonStyle {}
            text: "Close"

            onClicked: stackView.pop();
        }
    }
}
