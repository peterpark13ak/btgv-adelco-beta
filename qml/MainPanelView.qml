import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

Item {
    id: mainPanel

    // properties
    // -------------------------------------------------------------
    property bool   imagesPaused: false
    property string pathRNDSufix: ""
    property int fullScreenMode: 1
    property string pathFailRNDSufix: ""
    property double _FIRSTCOLPERC: 0.6

    // visual properties
    // -------------------------------------------------------------
    width:  parent.width
    height: parent.height

    // signals
    // -------------------------------------------------------------
    signal menuItemSelected(string menuItemId)
    signal imageUpdated(bool passed)

    // event handling
    // -------------------------------------------------------------
    Component.onCompleted: {
        statusTimer.running = true;
        mainWindow.onStatusPlcRunningChanged.connect(runningChanged);
        uiController.newMessage.connect(newMessage);
        uiController.imageReady.connect(imageUpdated);
    }

    // .............................................................
    Component.onDestruction: {
        statusTimer.running = false;
    }

    // .............................................................
    onImageUpdated: {
        if (imagesPaused)
            return;

        pathRNDSufix = "-" + Math.random(10);
        if (!passed) {
            pathFailRNDSufix = "-" + Math.random(10);
        }
        console.log("update Image");
    }

    // .............................................................
    function runningChanged() {
        startButton.confirm(statusPlcRunning);

        delayedExecution.acknowledge(statusPlcRunning ? "runMode" : "editMode")
    }

    // .............................................................
    function newMessage(message) {
        messageArea.newMessage(message);
    }

    // PAGES
    // -------------------------------------------------------------
    property Component editBatchConfig: BatchConfigurationView {
        onScreenReady: {
            delayedExecution.acknowledge("edit");
        }
    }

    // Timer to ask for plc status
    // ---------------------------------------------------------------------
    Timer {
        id: statusTimer
        running: false
        repeat: true
        interval: pollingPeriod
        onTriggered: uiController.requestPLCStatus();
    }

    // .............................................................
    ColumnLayout {
        anchors.fill: parent

        // TOP MENU -----------------------------------------------------------------------
        TopMenu {
            Layout.alignment: Qt.AlignTop
            Layout.bottomMargin: mainPanel.height / 80;
            Layout.fillWidth: true
            height: mainPanel.height / 12;

            onItemSelected: {
                menuItemSelected(menuItem);
            }
        }

        // MAIN PANEL ---------------------------------------------------------------------
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: _uimButtonHeight * 0.4

            // INSPECTION IMAGES ----------------------------------------------------------
            ColumnLayout {
                id: inspectionControls

                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: mainPanel.width * _FIRSTCOLPERC
                Layout.fillHeight: true

                Rectangle {
                    id: imagewrapper

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    color: "transparent"
                    border.color: "#666666"
                    border.width: 1
                    Layout.alignment: Qt.AlignCenter
                    Layout.margins: 10

                    RowLayout {
                        spacing: 0
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            ColumnLayout {
                                spacing: 0
                                    Image {
                                        id: inspectedSample1
                                        Layout.alignment: Qt.AlignHCenter
                                        Layout.preferredWidth: imagewrapper.width * 0.5
                                        Layout.preferredHeight: imagewrapper.height * 0.5

                                        cache: false
                                        asynchronous: false
                                        source: "image://buffer1/snapshot" + mainPanel.pathRNDSufix
                                        fillMode: Image.Stretch
                                        mipmap: true
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                fullScreenMode = 1;
                                                inspectedSample1.visible = false;
                                                inspectedSample2.visible = false;
                                                inspectedSample3.visible = false;
                                                inspectedSample4.visible = false;
                                                fullScreen.visible = true;
                                            }
                                        }
                                    }
                                Image {
                                    id: inspectedSample2
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.preferredWidth: imagewrapper.width * 0.5
                                    Layout.preferredHeight: imagewrapper.height * 0.5

                                    cache: false
                                    asynchronous: false

                                    source: "image://buffer2/snapshot" + mainPanel.pathRNDSufix
                                    fillMode: Image.Stretch
                                    mipmap: true
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            fullScreenMode = 2;
                                            inspectedSample1.visible = false;
                                            inspectedSample2.visible = false;
                                            inspectedSample3.visible = false;
                                            inspectedSample4.visible = false;
                                            fullScreen.visible = true;
                                        }
                                    }
                                }
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            ColumnLayout {
                                spacing: 0
                                Image {
                                    id: inspectedSample3
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.preferredWidth: imagewrapper.width * 0.5
                                    Layout.preferredHeight: imagewrapper.height * 0.5

                                    cache: false
                                    asynchronous: false

                                    source: "image://buffer3/snapshot" + mainPanel.pathRNDSufix
                                    fillMode: Image.Stretch
                                    mipmap: true
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            fullScreenMode = 3;
                                            inspectedSample1.visible = false;
                                            inspectedSample2.visible = false;
                                            inspectedSample3.visible = false;
                                            inspectedSample4.visible = false;
                                            fullScreen.visible = true;
                                        }
                                    }
                                }
                                Image {
                                    id: inspectedSample4
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.preferredWidth: imagewrapper.width * 0.5
                                    Layout.preferredHeight: imagewrapper.height * 0.5

                                    cache: false
                                    asynchronous: false

                                    source: "image://buffer4/snapshot" + mainPanel.pathRNDSufix
                                    fillMode: Image.Stretch
                                    mipmap: true
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            fullScreenMode = 4;
                                            inspectedSample1.visible = false;
                                            inspectedSample2.visible = false;
                                            inspectedSample3.visible = false;
                                            inspectedSample4.visible = false;
                                            fullScreen.visible = true;
                                        }
                                    }
                                }
                            }
                        }

                    }

                    Image {
                        id: previewSample

                        anchors.fill: parent
                        anchors.margins: 5

                        cache: false
                        asynchronous: false

                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        visible: false
                    }
                    Image {
                        id: fullScreen

                        anchors.fill: parent
                        anchors.margins: 5

                        cache: false
                        asynchronous: false

                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        visible: false
                        source: "image://buffer" + mainPanel.fullScreenMode +"/snapshot" + mainPanel.pathRNDSufix
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                fullScreen.visible = false;
                                fullScreenMode = 1;
                                inspectedSample1.visible = true;
                                inspectedSample2.visible = true;
                                inspectedSample3.visible = true;
                                inspectedSample4.visible = true;
                            }
                        }
                    }

                }

                // INSPECTION IMAGEs CONTROL BUTTONS ----------------------------------------------
                RowLayout {
                    spacing: 10

                    Layout.preferredHeight: mainPanel.height / 12;
                    Layout.fillWidth: true

                    Button {
                        id: pausebutton
                        enabled: ! imagesPaused
                        opacity: enabled ? 0.8 : 0.4
                        iconSource: "../images/pause.png"
                        style: BlackButtonStyle { }
                        Layout.preferredHeight: mainPanel.height / 12;
                        onClicked: {
                            imagesPaused = true;
                        }
                    }
                    Button {
                        id: playbutton
                        enabled: imagesPaused
                        opacity: enabled ? 0.8 : 0.4
                        iconSource: "../images/play.png"
                        style: BlackButtonStyle { }
                        Layout.preferredHeight: mainPanel.height / 12;
                        onClicked: {
                            imagesPaused = false;
                            previewSample.visible = false;
                            inspectedSample1.visible = true;
                            inspectedSample2.visible = true;
                            inspectedSample3.visible = true;
                            inspectedSample4.visible = true;
                            imageUpdated(false);
                            imagewrapper.border.color = "#666666";
                        }
                    }
                    Button {
                        id: clearbutton
                        enabled: ! statusPlcRunning
                        visible: ! statusPlcRunning
                        opacity: enabled ? 0.8 : 0.4
                        iconSource: "../images/remove.png"
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: mainPanel.height / 12;
                        onClicked: {
                            uiController.clearFailedImages();
                            pathFailRNDSufix = "-" + Math.random(10);
                        }
                    }
                    Button {
                        id: resultsbutton
                        enabled: statusPlcRunning
                        visible: statusPlcRunning
                        opacity: enabled ? 0.8 : 0.4
                        iconSource: "../images/results.png"
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: mainPanel.height / 12;
                        onClicked: {
                            menuItemSelected("MENU_RESULTS");
                        }
                    }
                    Rectangle {
                        color: "transparent"
                        height: 1
                        Layout.fillWidth: true
                    }

                    // Edit
                    // -------------------------------------------------------------
                    Button {
                        id: editbutton
                        enabled: ! statusPlcRunning

                        style: BlackButtonStyle {}
                        iconSource: "../images/edit.png"
                        Layout.preferredHeight: mainPanel.height / 12;

                        text: activeConfigId

                        onClicked: {
                            delayedExecution.execute(
                                "edit",
                                (function(activeConfigId) {
                                    stackView.push({item: editBatchConfig, properties: { configId: activeConfigId } });
                                    stackView.currentItem.forceActiveFocus();
                                })
                                .bind(this, activeConfigId),
                                true
                            );
                        }
                    }
                }

                // Failed Images
                // -------------------------------------------------------------------------
                Rectangle {
                    color: "transparent"

                    Layout.fillWidth: true
                    Layout.preferredHeight: (failedImages.width - 40) / 5

                    ListView {
                        id: failedImages

                    spacing: 10
                    anchors.fill: parent


                        model: 5

                        orientation: ListView.Horizontal

                        delegate: Rectangle {
                            Layout.alignment: Qt.AlignTop

                            height: width
                            //Layout.fillWidth: true
                            width: (failedImages.width - 40) / 5

                            border.width: 1
                            border.color: "#666666"
                            color: "transparent"

                            Image {
                                anchors.margins : 4
                                anchors.fill: parent

                                cache: false
                                asynchronous: false

                                source: "image://buffer/f" + index + mainPanel.pathFailRNDSufix;
                                fillMode: Image.PreserveAspectFit
                                mipmap: true

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (!imagesPaused) return;
                                        imageUpdated(false);
                                        previewSample.source = parent.source;
                                        previewSample.visible = true;
                                        inspectedSample1.visible = false;
                                        inspectedSample2.visible = false;
                                        inspectedSample3.visible = false;
                                        inspectedSample4.visible = false;
                                        imagewrapper.border.color = "#ff6666";
                                        console.log(parent.source);
                                    }
                                }
                            }
                        }
                    }

                }

                MessageView {
                    id: messageArea

                    anchors.bottom: parent.bottom
                    Layout.fillWidth: true
                    Layout.preferredHeight: mainPanel.height / 12
                    openHeight: mainPanel.height / 2
                    opacity: 0.9
                }
            }

            // STATUS PANEL ---------------------------------------------------------------
            ColumnLayout {
                id: statusMonitor
                Layout.preferredWidth: mainPanel.width * 0.4
                Layout.alignment: Qt.AlignTop | Qt.AlignRight
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 10


                RowLayout {
                    id: statusControls

                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredHeight: _uimButtonHeight // mainPanel.height * 2 / 12
                    Layout.bottomMargin: _uimButtonHeight * 0.1

                    spacing: _uimButtonHeight * 0.2

                    IndicatorLed {
                        Layout.preferredWidth: _uimButtonHeight
                        Layout.preferredHeight: _uimButtonHeight

                        id: runningStatus
                        name: "Running"
                        height: _uimButtonHeight
                        width: height
                        on: statusPlcRunning
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    IndicatorLed {
                        Layout.preferredWidth: _uimButtonHeight
                        Layout.preferredHeight: _uimButtonHeight

                        id: warningStatus
                        name: "Warning"
                        height: _uimButtonHeight
                        width: height
                        indicatorColor: "#f0a30a"
                        on: statusPlcWarning
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    IndicatorLed {
                        Layout.preferredWidth: _uimButtonHeight
                        Layout.preferredHeight: _uimButtonHeight

                        id: errorStatus
                        name: "Error"
                        height: _uimButtonHeight
                        width: height
                        indicatorColor: "red"
                        on: statusPlcError
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        Layout.fillWidth: true
                    }

                    ToggleDelayButton {
                        id: startButton

                        Layout.preferredWidth: _uimButtonHeight * 1.5
                        Layout.preferredHeight: _uimButtonHeight * 1.5

                        enabled: statusPlcConnected
                        turnedOn: statusPlcRunning

                        height: mainPanel.height * 1.5 / 12
                        // width: height
                        anchors.verticalCenter: parent.verticalCenter

                        onActivated: {
                            mainWindow.statusJobReady = false;

                            if (turnedOn === false) {
                                // run mode requests reloading (download) of Job to camera
                                // check for the necessity of asynchronous response
                                delayedExecution.execute(
                                    "runMode",
                                    (function() {
                                        uiController.setRunMode();
                                        uiController.requestPLCStart();
                                        //this.enabled = false;  // sometimes the button remains disabled !?
                                    })
                                    .bind(this),
                                    true
                                );
                            }
                            else {
                                delayedExecution.execute(
                                    "editMode",
                                    (function() {
                                        uiController.requestPLCStop();
                                        uiController.setEditMode();
                                        //this.enabled = false;  // sometimes the button remains disabled !?
                                    })
                                    .bind(this),
                                    true
                                );
                            }
                        }
                    }

                }

                TabView {
                    id: counterLists
                    objectName: "counterLists"

                    Layout.alignment: Qt.AlignTop
                    Layout.preferredWidth: mainPanel.width * 0.25
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // clip: true

                    // width: statusMonitor.width

                    style: TouchTabViewStyle {
                        tabWidth: statusMonitor.width * 0.5
                        tabHeight: mainPanel.height / 12
                    }

                    Tab {
                        title: "Batch"

                        anchors.fill: parent

                        CounterList {
                            anchors.fill: parent
                            prefix: "batch";
                            fontSize: mainPanel.height * 0.25 / 12
                            isActive: counterLists.currentIndex === 0
                        }
                    }
                    Tab {
                        title: "Daily"

                        anchors.fill: parent

                        CounterList {
                            prefix: "daily";
                            fontSize: mainPanel.height * 0.25 / 12
                            isActive: counterLists.currentIndex === 1
                        }
                    }
//                    Tab {
//                        title: "Global"

//                        CounterList {
//                            prefix: "global";
//                            fontSize: mainPanel.height * 0.25 / 12
//                            isActive: counterLists.currentIndex === 2
//                        }
//                    }
                }

                RowLayout {
                    Button {
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: mainPanel.height / 12
                        text: "Errors"
                        enabled: statusPlcError || statusPlcWarning
                        onClicked: {
                            menuItemSelected("MENU_ERRORS");
                        }
                    }

                    Rectangle { Layout.fillWidth: true }

                    Button {
                        enabled: statusPlcConnected
                        style: BlackButtonStyle { }
                        Layout.preferredHeight: mainPanel.height / 12
                        text: "Clear Counters"
                        onClicked: {
                            uiController.resetCounters(counterLists.currentIndex);
                        }
                    }
                }


            }

        }
    }

    // Delay Execution
    // -----------------------------------------------------------------------
    DelayedExecution {
        id: delayedExecution

        onCancel: {
            console.log("canceling execution (start) wait");
            stackView.pop();
        }
    }
}
