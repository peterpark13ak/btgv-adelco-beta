import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import com.darebit.blisterocv 1.0

Rectangle {
    id: mainWindow

    // status properties
    // -------------------------------------------------------------
    property bool statusPlcConnected: false
    property bool statusPlcRunning: false
    property bool statusPlcError: false
    property bool statusPlcWarning: false
    property bool statusPlcDataToggle: false                // used to mask multiple systemStatus changed notify events
    property bool statusJobReady: uiController.isJobReady()

    property SystemStatus plcStatusData
    property string activeConfigId

    // external events
    // -------------------------------------------------------------
    signal plcConnect();
    signal plcDisconnect();
    signal plcStatus(SystemStatus status);
    signal plcStarted();
    signal plcStopped();
    signal plcErrorStatus(bool errorStatus);
    signal plcWarningStatus(bool warningStatus);

    signal jobReady(bool ready);

    signal newMessage(string message);

    // visual properties
    // -------------------------------------------------------------
    color: "#161616"
    anchors.fill: parent

    property int _uimWidth: width
    property int _uimHeight: height
    property int _uimButtonHeight: height / 12
    property int _uimH1FontSize: height / 36
    property int _uimFontSize: height / 48

    // configuration properties
    // -------------------------------------------------------------
    property int pollingPeriod: 2000

    // external event handlers
    // -------------------------------------------------------------
    Component.onCompleted: {
        uiController.plcConnect.connect(plcConnect);
        uiController.plcDisconnect.connect(plcDisconnect);
        uiController.plcStarted.connect(plcStarted);
        uiController.plcStopped.connect(plcStopped);
        uiController.plcErrorStatus.connect(plcErrorStatus);
        uiController.plcWarningStatus.connect(plcWarningStatus);
        uiController.plcStatus.connect(plcStatus);

        uiController.jobReady.connect(jobReady);
    }

    // Handle Job events
    // -------------------------------------------------------------
    onJobReady: {
        statusJobReady = ready;
        if (ready === true) {
            activeConfigId = uiController.getActiveConfigurationId();

            console.log("Active conf id: " + activeConfigId);
        }
    }

    // handle PLC start - stop
    // -------------------------------------------------------------
    onPlcStarted: {
        statusPlcRunning = true;
    }

    onPlcStopped: {
        statusPlcRunning = false;
    }

    // handle PLC connection changes
    // -------------------------------------------------------------
    onPlcConnect: {
        statusPlcConnected = true;
    }

    onPlcDisconnect: {
        statusPlcConnected = false;
    }

    // handle PLC status messages
    // -------------------------------------------------------------
    onPlcStatus: {
        plcStatusData = status;
        statusPlcDataToggle = ! statusPlcDataToggle;
    }

    // handle PLC error and warning status changes
    // -------------------------------------------------------------
    onPlcErrorStatus: {
        statusPlcError = errorStatus;
    }

    onPlcWarningStatus: {
        statusPlcWarning = warningStatus;
    }

    // handle messages
    // -------------------------------------------------------------
    onNewMessage: {
        messageArea.newMessage(message);
    }

    // -------------------------------------------------------------
    // define signals
    // -------------------------------------------------------------
    signal newSampleTrigger(int seq);
    signal imageUpdated(bool passed);
    signal newStatistics(string statValues);

    // PAGES
    // -------------------------------------------------------------
    // -------------------------------------------------------------

    property Component batchConfiguration: ConfigListView { }

    // Statistis page
    // -------------------------------------------------------------
    property Component resultsList: ResultsView { }

    // Errors page
    // -------------------------------------------------------------
    property Component errorsList: ErrorsView { }

    // System Configuration page
    // -------------------------------------------------------------
    property Component systemConfiguration: SystemConfigurationView { }

    // System Test page
    // -------------------------------------------------------------
    property Component systemTest: SystemTestView { }

    // Main Panel page
    // -------------------------------------------------------------
    property Component mainPanel: MainPanelView {

        onMenuItemSelected: {
            switch (menuItemId) {

            case "MENU_BATCH_CONFIGURATION":
                if (stackView.depth == 1) {
                    stackView.push({ item: batchConfiguration });
                    stackView.currentItem.forceActiveFocus();
                }
                break;

            case "MENU_SYSTEM_CONFIGURATION":
                if (stackView.depth == 1) {
                    stackView.push({ item: systemConfiguration });
                    stackView.currentItem.forceActiveFocus();
                }
                break;

            case "MENU_SYSTEM_TEST":
                if (stackView.depth == 1) {
                    stackView.push({ item: systemTest });
                    stackView.currentItem.forceActiveFocus();
                }
                break;

            case "MENU_ERRORS":
                if (stackView.depth == 1) {
                    stackView.push({ item: errorsList });
                    stackView.currentItem.forceActiveFocus();
                }
                break;

            case "MENU_RESULTS":
                if (stackView.depth == 1) {
                    stackView.push({item: resultsList });
                    stackView.currentItem.forceActiveFocus();
                }
                break;

            case "MENU_CLOSE":
                closeDialog.visible = true;
                break;
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        anchors.margins: _uimButtonHeight * 0.2
        initialItem: mainPanel
    }

    onNewStatistics: {
        if (stackView.currentItem.newStatistics !== undefined) {
            stackView.currentItem.newStatistics(statValues);
        }
    }

    onImageUpdated: {
        if (stackView.currentItem.imageUpdated !== undefined) {
            stackView.currentItem.imageUpdated(passed);
        }
    }

    // text to help getting typography metrics
    // -------------------------------------------------------------
    Text { id: textSingleton }

    // Load fonts
    // -------------------------------------------------------------
    FontLoader {
        id: openSans
        source: "qrc:/fonts/OpenSans-Regular.ttf"
     }

    FontLoader {
        id: adventPro
        source: "qrc:/fonts/AdventPro-SemiBold.ttf"
     }

    // Wait Cursor - Busy Indicator
    // -------------------------------------------------------------
    Rectangle {
        anchors.fill: parent
        z: 100
        color: "#88000000"
        visible: busyIndicator.running

        MouseArea {
            anchors.fill: parent
            onClicked: {
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
            running: statusJobReady === false
            width: _uimButtonHeight
            height: _uimButtonHeight
            anchors.centerIn: parent
        }
    }

    // Application Close dialog
    // -------------------------------------------------------------
    CloseDialog {
        id: closeDialog
    }

    // Delay Execution
    // -----------------------------------------------------------------------
    DelayedExecution {
        id: rootDelayedExecution

        onCancel: {
            console.log("canceling execution (start) wait");
            stackView.pop();
        }
    }
}
