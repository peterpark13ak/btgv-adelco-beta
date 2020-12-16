import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

ColumnLayout {
    id: errorsList

    Layout.fillHeight: true
    Layout.fillWidth: true

    property int errorFlags

    Component.onCompleted: {
        errorFlags = mainWindow.plcStatusData.errorFlags.charCodeAt(0) | (mainWindow.plcStatusData.errorFlags.charCodeAt(1) << 8);
    }

    FormHeader {
        Layout.fillWidth: true
        Layout.preferredHeight: _uimButtonHeight
        Layout.bottomMargin: _uimButtonHeight * 0.4
        text: "Errors and Warnings"
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true

        spacing: _uimButtonHeight * 0.4

        Repeater {
            //#define SYSTEM_ERROR_A_CONSECUTIVE_NOANSWERS 0x01
            // -- #define SYSTEM_ERROR_A_CONSECUTIVE_FAILES 0x02
            //#define SYSTEM_ERROR_A_BLISTER_DOUBLEID 0x04
            //#define SYSTEM_ERROR_A_BUFFER_FULL 0x08
            //#define SYSTEM_ERROR_A_RESERVED 0x10
            // -- #define SYSTEM_ERROR_A_STEP1_ERROR 0x20
            // -- #define SYSTEM_ERROR_A_STEP2_ERROR 0x40
            //#define SYSTEM_ERROR_A_RESERVED2 0x80

            //#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER1_FAILES  0x0100
            //#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER2_FAILES  0x0200
            //#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER3_FAILES  0x0400
            //#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER4_FAILES  0x0800


            model: [
                { description: "consecutive missing answers above threshold",   mask: 0x01 }
                //,{ description: "consecutive falied answers above threshold",    mask: 0x02 }
                ,{ description: "blisters with the same ID",                     mask: 0x04 }
                ,{ description: "the blister buffer is full",                    mask: 0x08 }
                //,{ description: "block at rejection step 1",                     mask: 0x20 }
                //,{ description: "block at rejection step 2",                     mask: 0x40 }
                ,{ description: "consecutive failed blister #1",                 mask: 0x0100 }
                ,{ description: "consecutive failed blister #2",                 mask: 0x0200 }
                ,{ description: "consecutive failed blister #3",                 mask: 0x0400 }
                ,{ description: "consecutive failed blister #4",                 mask: 0x0800 }
            ]

            delegate: Text {
                Layout.preferredWidth: _uimButtonHeight * 10
                Layout.margins: _uimButtonHeight * 0.2
                color: "#f0a30a"
                font.pixelSize: _uimFontSize * 1.3

                text: "- " + modelData.description
                visible: errorsList.errorFlags & modelData.mask
            }
        }
    }

    // Action Buttons
    RowLayout {
        spacing: _uimButtonHeight * 0.2

        Button {
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle {}
            text: "Clear & Close"

            onClicked: {
                uiController.resetErrors();
                stackView.pop();
            }
        }

        Button {
            Layout.preferredHeight: _uimButtonHeight
            style: BlackButtonStyle {}
            text: "Close"

            onClicked: stackView.pop();
        }
    }
}
