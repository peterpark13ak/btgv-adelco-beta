import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

ColumnLayout {
    id: systemConfiguration

    // properties
    // -------------------------------------------------------------
    property SystemConfiguration configuration: SystemConfiguration {}
    property Component numericKeypad: NumericKeypadView {}

    // visual properties
    // -------------------------------------------------------------
    Layout.fillWidth: true
    Layout.fillHeight: true

    // event handlers
    // -------------------------------------------------------------
    Component.onCompleted: {
        // fill in the systemConfiguration property with the current values
        configuration = uiController.systemConfiguration;

        noAnswerThreshold.value = configuration.noAnswerThreshold;
        failedThreshold.value = configuration.failedThreshold;
        continueOnError.checked = configuration.continueOnError;
    }

    FormHeader {
        Layout.preferredHeight: _uimButtonHeight
        Layout.fillWidth: true
        Layout.bottomMargin: _uimButtonHeight / 4
        text: "System Configuration"
    }

    RowLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: _uimButtonHeight / 4

        // first column
        // -------------------------------------------------------------
        ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: _uimButtonHeight / 2

            NumberInputField {
                id: noAnswerThreshold
                label: "No Answer Threshold:"
                labelWidth: 4 * _uimButtonHeight
                onSelected: {
                    systemConfiguration.Stack.view.push({
                        item: numericKeypad,
                        properties: {
                            title: "No Answer Threshold",
                            target: noAnswerThreshold
                        }
                    });
                }
            }

            NumberInputField {
                id: failedThreshold
                label: "Failed Blisters Threshold:"
                labelWidth: 4 * _uimButtonHeight
                onSelected: {
                    systemConfiguration.Stack.view.push({
                        item: numericKeypad,
                        properties: {
                            title: "Failed Blisters Threshold",
                            target: failedThreshold }
                    });
                }
            }

            RowLayout {
                spacing:  _uimButtonHeight * 0.6

                function updateEncoder(status) {
                    encoderValue.text = status.encoder;
                }

                Component.onCompleted: {
                    uiController.plcStatus.connect(updateEncoder);
                }

                Component.onDestruction: {
                    uiController.plcStatus.disconnect(updateEncoder);
                }

                Text {
                    Layout.preferredWidth: 4 * _uimButtonHeight
                    text: "Encoder current value:"
                    color: "#999999"
                    font.pixelSize: 14
                }

                Text {
                    id: encoderValue
                    Layout.preferredWidth: 60
                    font.pixelSize: 14
                    text: "waiting ..."

                    color: "white"
                }
            }

            function editNumber(label, target) {
                systemConfiguration.Stack.view.push({
                    item: numericKeypad,
                    properties: {
                        title: label,
                        target: target }
                });
            }

            // Continue on errors
            // ----------------------------------------------------------------------
            RowLayout {
                Layout.fillWidth: true
                spacing:  _uimButtonHeight / 2

                Text {
                    Layout.preferredWidth: 4 * _uimButtonHeight
                    text: "Continue on Errors:"
                    color: "#999999"
                    font.pixelSize: _uimFontSize
                }

                Switch {
                    id: continueOnError
                    style: BlackSwitchStyle {}
                    checked: configuration.continueOnError
                }
            }

        }
    }

    // form action buttons
    // -------------------------------------------------------------
    RowLayout {
        spacing: _uimButtonHeight * 0.2

        Button {
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle {}
            text: "Close"

            onClicked: stackView.pop();
        }

        Button {
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle {}
            text: "Use current Values"

            onClicked: {
                configuration.noAnswerThreshold = noAnswerThreshold.value;
                configuration.failedThreshold = failedThreshold.value;
                configuration.continueOnError = continueOnError.checked;

                uiController.saveSystemConfiguration(configuration);

                stackView.pop();
            }
        }
    }
}
