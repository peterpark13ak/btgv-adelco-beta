import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

ColumnLayout {
    id: systemTest

    // visual properties
    // -------------------------------------------------------------
    Layout.fillWidth: true
    Layout.fillHeight: true

    FormHeader {
        Layout.preferredHeight: _uimButtonHeight
        Layout.fillWidth: true
        Layout.bottomMargin: _uimButtonHeight / 4
        text: "System Test"
    }

    RowLayout {
        Text {
            Layout.preferredWidth: 3 * _uimButtonHeight
            text: "Manual Mode:"
            color: "#999999"
            font.pixelSize: _uimFontSize
        }

        Switch {
            id: manualMode
            style: BlackSwitchStyle {}
            onCheckedChanged: {
                uiController.setManualMode( manualMode.checked );
            }
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true

        spacing: _uimButtonHeight * 0.5

        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: _uimButtonHeight / 4

            Repeater {
                model: [
                    { title: "Reject Blister LED",      code:  1 },
                    { title: "Machine Stop",            code:  2 },
                    { title: "Trigger to Camera",       code:  4 },
                    { title: "Strobe Light",            code:  5 },
                    { title: "Single Triger",           code:  6 },
                    { title: "Multiple Triger",         code:  7 }
                ]

                delegate: RowLayout {
                    Text {
                        Layout.preferredWidth: 3 * _uimButtonHeight
                        text: modelData.title
                        color: "#999999"
                        font.pixelSize: _uimFontSize
                    }

                    Button {
                        id: testButton
                        Layout.preferredHeight: _uimButtonHeight
                        enabled: manualMode.checked
                        style: BlackButtonStyle {
                            pressColor: testButton.pressed ? "#411" : "#222"
                        }
                        text: "Keep Pressed to Test"
                        onPressedChanged: {
                            if (testButton.pressed) {
                                uiController.startTest(modelData.code);
                            }
                            else {
                                uiController.stopTest(modelData.code);
                            }
                        }
                    }
                }
            }
        }


        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: _uimButtonHeight / 4

            Repeater {
                model: [
                     { title: "Reject Blister Valve#1",  code: 11 }
                    ,{ title: "Reject Blister Valve#2",  code: 12 }
                    //,{ title: "Reject Blister Valve#3",  code: 13 }
                    //,{ title: "Reject Blister Valve#4",  code: 14 }
                ]

                delegate: RowLayout {
                    Text {
                        Layout.preferredWidth: 3 * _uimButtonHeight
                        text: modelData.title
                        color: "#999999"
                        font.pixelSize: _uimFontSize
                    }

                    Button {
                        id: testButton
                        Layout.preferredHeight: _uimButtonHeight
                        enabled: manualMode.checked
                        style: BlackButtonStyle {
                            pressColor: testButton.pressed ? "#411" : "#222"
                        }
                        text: "Keep Pressed to Test"
                        onPressedChanged: {
                            if (testButton.pressed) {
                                uiController.startTest(modelData.code);
                            }
                            else {
                                uiController.stopTest(modelData.code);
                            }
                        }
                    }
                }
            }
        }
    }

    // form action buttons
    // -------------------------------------------------------------
    RowLayout {
        Button {
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle {}
            text: "Close"

            onClicked: {
                uiController.setManualMode(false);
                stackView.pop();
            }
        }
    }
}
