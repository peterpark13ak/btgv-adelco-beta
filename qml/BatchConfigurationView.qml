import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

Item {
    id: batchConfiguration

    // properties
    // -------------------------------------------------------------
    property BatchConfiguration batchConfig

    property string configId
    property var    parameterList;
    property var    globalDatums
    property bool   waitingImage:   false
    property var    readOnlyConfig: uiController.getReadOnlyConfigurations()    // get list of read only configurations
    property string pathRNDSufix:   ""
    property int    trainingInspectionID: 1

    // visual properties
    // -------------------------------------------------------------
    width:  parent.width
    height: parent.height

    // signals
    // -------------------------------------------------------------
    signal imageReady();

    signal screenReady();
    signal dataSubmitted(BatchConfiguration configData);
    signal imageUpdated(bool passed)

    // Event handling
    // -------------------------------------------------------------
    function refreshImage() {
        pathRNDSufix = "-" + Math.random(10);
        inspectedSample.source = "image://train" + trainingInspectionID + "/snapshot"  + pathRNDSufix;
    }

    // .............................................................
    Timer {
        id: acquireDelay

        interval: 10
        repeat: false

        onTriggered: {
            waitingImage = true;
            console.log("*********************** AcuireImage");
            uiController.acquireImage();
        }
    }

    // .............................................................
    onImageReady: {
        delayedExecution.acknowledge("acqImage");

        waitingImage = false;
        pathRNDSufix = "-" + Math.random(10);
        inspectedSample.source = "image://train" + trainingInspectionID + "/snapshot"  + pathRNDSufix;

        console.log(inspectedSample.source);

        acquireDelay.start();
    }

    // initialize the screen - call from onComplete event
    // .............................................................
    function initialize() {
        // load AVP
        uiController.loadAvp(configId);

        uiController.setManualMode(true);
        uiController.startTest(7);          // 7: Triggers in manual mode

        batchConfig = uiController.getConfigurationById(configId);
        parameterList = [];

        var gDatumsJson = uiController.getGlobalDatums(trainingInspectionID - 1);
        globalDatums = JSON.parse(gDatumsJson);

        // append the global Datums
        for (var i = 0; i < globalDatums.length; i++) {
            parameterList.push({
                                   "type": "avpParam",
                                   "index": i,
                                   "code": globalDatums[i].name,
                                   "label": globalDatums[i].label,
                                   "minValue": globalDatums[i].minValue,
                                   "maxValue": globalDatums[i].maxValue,
                                   "data": globalDatums[i].value
                               });
        }

        parameterList.push({   "type": "plcParam",
                               "code": "cameraToReject",
                               "label": "Camera to Reject (No of Pulses)",
                               "minValue": 0,
                               "maxValue": 100,
                               "data": "" + batchConfig.cameraToReject
                           });
        parameterList.push({   "type": "plcParam",
                               "code": "trigger1Offset",
                               "label": "Camera shot #1 delay (pulses)",
                               "minValue": 0,
                               "maxValue": 10000,
                               "data": "" + batchConfig.trigger1Offset
                          });
        parameterList.push({   "type": "plcParam",
                               "code": "trigger2Offset",
                               "label": "Camera shot #2 delay (pulses)",
                               "minValue": 0,
                               "maxValue": 100000,
                               "data": "" + batchConfig.trigger2Offset
                          });
        parameterList.push({   "type": "plcParam",
                               "code": "reject1Offset",
                               "label": "Rejection delay (pulses)",
                               "minValue": 0,
                               "maxValue": 10000,
                               "data": "" + batchConfig.reject1Offset
                           });
        parameterList.push({   "type": "plcParam",
                               "code": "rejectDistance",
                               "label": "Subsequent rejection distance (pulses)",
                               "minValue": 0,
                               "maxValue": 10000,
                               "data": "" + batchConfig.rejectDistance
                           });
        parameterList.push({   "type": "plcParam",
                               "code": "rejectDuration",
                               "label": "Rejection duration (pulses)",
                               "minValue": 0,
                               "maxValue": 10000,
                               "data": "" + batchConfig.rejectDuration
                           });

//        parameterList.push({   "type": "plcParam",
//                               "code": "noOfProducts",
//                               "label": "Number of products",
//                               "minValue": 1,
//                               "maxValue": 4,
//                               "data": "" + batchConfig.noOfProducts
//                           });

        parameters.model = parameterList;

        // connect trainImageReady signal after initialization has been finished
        uiController.trainImageReady.connect(imageReady);
        uiController.setEditMode();
        screenReady();
    }

    // .............................................................
    Component.onCompleted: {
        delayedExecution.execute(
            "init",
            (function() {
                initialize();

                delayedExecution.execute(
                    "acqImage",
                    (function() {
                        waitingImage = true;
                        uiController.acquireImage();
                    })
                    .bind(this),
                    true            // wait acknowledge
                );
            })
            .bind(this)
        );
    }

    // .............................................................
    Component.onDestruction: {
        uiController.trainImageReady.disconnect(imageReady);
        uiController.cancelAcquireImage();

        uiController.stopTest(7);           // 7: Triggers in manual mode
        uiController.setManualMode(false);
    }

    // PAGES
    // -------------------------------------------------------------
    property Component editAvpView: AvpEditView {
        Component.onDestruction: {
            // necessary to reconnect handler to trainImageReady signal
            // after returning from AvpEditView
            uiController.trainImageReady.connect(imageReady);

            uiController.acquireImage();
            batchConfiguration.waitingImage = true;
        }
    }

    // visual numeric keyboard
    // -------------------------------------------------------------
    property Component numericKeypad: NumericKeypadView {}

    // visual qwerty keyboard
    // -------------------------------------------------------------
    property Component stringKeypad: StringKeypadView {
        disabledKeys: [ "/", " ", ":", ";", "*", "=", "<", ">", "?"]
        validate: (function(name) {
            var exists = uiController.configurationNameExists(name);
            return {
                valid: ! exists,
                message: exists ? "The name exists. Use a different name" : "ok"
            }
        })

        onSave: {
            // Save As implementation
            // ---------------------------------------------------------------
            delayedExecution.execute(
                "saveAs",
                (function(configId, config) {

                    config.name = configId;
                    config.avpFileName = configId + ".avp";

                    uiController.saveAvp(configId);
                    uiController.saveConfiguration(config);

                    stackView.pop();
                })
                .bind(this, value, batchConfig)
            );

        }
    }

    // .............................................................
    onImageUpdated: {
        pathRNDSufix = "-" + Math.random(10);
    }

    // .............................................................
    function editNumber(label, target) {
        batchConfiguration.Stack.view.push({
            item: numericKeypad,
            properties: {
                title: label,
                target: target }
        });
    }

    // .............................................................
    // use the current configuration and load the AVP file
    function useConfig() {
        uiController.useBatchConfiguration(batchConfig, true);
        configId = batchConfig.name;
    }

    // .............................................................
    ColumnLayout {

        anchors.fill: parent

        spacing: _uimButtonHeight * 0.2

        // Page Layout
        // -------------------------------------------------------------
        FormHeader {
            Layout.preferredHeight: _uimButtonHeight
            Layout.fillWidth: true
            text: "Batch Configuration (" + configId + ")"
        }

        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 0

            spacing: _uimButtonHeight * 0.4

            Rectangle {
                id: imagewrapper

                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: _uimWidth * 0.6
                Layout.fillHeight: true
                Layout.fillWidth: true

                color: "transparent"
                border.color: "#666666"
                border.width: 1

                Image {
                    id: inspectedSample

                    anchors.fill: parent
                    anchors.margins: 2

                    cache: false
                    asynchronous: false

                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }

                RowLayout {
                    height: _uimButtonHeight

                    Button {
                        id: requestInspection1
                        iconSource: trainingInspectionID == 1 ? "../images/cam1_active.png" : "../images/cam1.png"
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            trainingInspectionID = 1;
                            initialize();
                        }
                    }
                    Button {
                        id: requestInspection2
                        style: BlackButtonStyle {}
                        iconSource: trainingInspectionID == 2 ? "../images/cam2_active.png" : "../images/cam2.png"
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            trainingInspectionID = 2;
                            initialize();
                        }
                    }
                    Button {
                        id: requestInspection3
                        style: BlackButtonStyle {}
                        iconSource: trainingInspectionID == 3 ? "../images/cam3_active.png" : "../images/cam3.png"
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            trainingInspectionID = 3;
                            initialize();
                        }
                    }
                    Button {
                        id: requestInspection4
                        iconSource: trainingInspectionID == 4 ? "../images/cam4_active.png" : "../images/cam4.png"
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            trainingInspectionID = 4;
                            initialize();
                       }
                    }

                }

                BusyIndicator {
                    id: imageWaitIndicator

                    width: _uimButtonHeight * 0.4
                    height: _uimButtonHeight * 0.4

                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: _uimButtonHeight * 0.1

                    running: waitingImage // && (delayedExecution.visible === false)
                }
            }

            ColumnLayout {
                id: parametersColumn

                Layout.alignment: Qt.AlignTop
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredWidth: _uimWidth * 0.4

                spacing: _uimButtonHeight * 0.2

                Component {
                    id: paramDelegate

                    NumberInputSpinner {
                        id: param

                        height: _uimButtonHeight * 1.1

                        label: modelData.label + ":"
                        minValue: modelData.minValue
                        maxValue: modelData.maxValue
                        value: modelData.data

                        onSelected: editNumber(modelData.label, param)

                        onAccepted: {
                            if (modelData.type === "plcParam") {
                                uiController.useBatchConfiguration(batchConfig, false);
                            } else if (modelData.type === "avpParam") {
                                var code = param.ListView.view.model[index].code;
                                uiController.setAvpNumericParam(trainingInspectionID - 1, code, param.value);
                            }
                        }

                        onChange: {
                            if (modelData.type === "plcParam") {
                                batchConfig[modelData.code] = value;
                            } else if (modelData.type === "avpParam") {
                                globalDatums[modelData.index].value = value;
                            }
                        }
                    }
                }

                ListView {
                    id: parameters

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    spacing: _uimButtonHeight * 0.4

                    delegate: paramDelegate
                }
            }
        }

        // form action buttons
        // ------------------------------------------------------------------------------------
        RowLayout {
            id: actionButtons

            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: _uimButtonHeight * 0.2

            // Save
            // ---------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                text: "Close"

                onClicked: {
                    stackView.pop();
                }
            }

            // spacer
            Rectangle {
                Layout.fillWidth: true
                height: 0
            }

            // Save
            // ---------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                enabled: batchConfig
                         && readOnlyConfig.indexOf(batchConfig.name) < 0
                text: "Save"

                onClicked: {
                    delayedExecution.execute(
                        "save",
                        (function(values, configId) {
                            uiController.updateConfiguration(values);
                            uiController.saveAvp(configId);
                        })
                        .bind(this, batchConfig, configId)
                    );
                }
            }

            // Save As
            // ---------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                text: "Save As"

                onClicked: {
                    batchConfiguration.Stack.view.push({
                        item: stringKeypad,
                        properties: {
                            title: "new config name",
                            value: ""
                        }
                    });
                }
            }

            // spacer
            Rectangle {
                Layout.fillWidth: true
                height: 0
            }

            // Edit Program
            // ---------------------------------------------------------------
            Button {
                id: editavp
                style: BlackButtonStyle {}
                Layout.preferredHeight: _uimButtonHeight
                text: "Edit Program"
                onClicked: {
                    // disconnect trainImageReady signal before getting to AvpEditView
                    // Remember to reconnect on return (see: AvpEditView.onDestruction above)
                    uiController.trainImageReady.disconnect(imageReady);

                    // TODO: check - do we need to do that? Is the same source signal for trainImageReady
                    // and the corresponding signal of AvpEditView? What is the cancelAcquireImage causing?
                    // Trace the handlers' execution simulating sparce triggers
                    if (waitingImage) {
                        uiController.cancelAcquireImage();
                    }

                    stackView.push( { item: editAvpView } );
                    stackView.currentItem.forceActiveFocus();
                }
            }

            // Use
            // ---------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                enabled: batchConfig &&
                         batchConfig.name !== activeConfigId
                text: "Use"

                onClicked: {
                    delayedExecution.execute(
                        "use",
                        (function() {
                            useConfig();
                            stackView.pop({ item: mainPanel });
                        })
                        .bind(this)
                    );
                }
            }

            // spacer
            Rectangle {
                Layout.fillWidth: true
                height: 0
            }

            // Delete
            // ---------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                text: "Delete"

                // disabled when the configuration is the active one
                // and when is marked as read only
                enabled: batchConfig &&
                         batchConfig.name !== activeConfigId
                         && readOnlyConfig.indexOf(batchConfig.name) < 0

                onClicked: {
                    delayedExecution.execute(
                        "delete",
                        (function(configId) {
                            uiController.deleteConfiguration(configId);
                            stackView.pop();
                        })
                        .bind(this, batchConfig.name)
                    );
                }
            }

        }
    }

    // Delay Execution
    // -----------------------------------------------------------------------
    DelayedExecution {
        id: delayedExecution

        onCancel: {
            stackView.pop();
        }
    }
}
