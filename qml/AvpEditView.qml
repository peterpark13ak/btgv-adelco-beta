import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import com.darebit.blisterocv 1.0

Item {
    id: avpEdit

    // visual properties
    // -------------------------------------------------------------
    width:  parent.width
    height: parent.height

    // properties
    // -------------------------------------------------------------
    property bool   waitingImage:   false           // guard variable to support image acquisition
    property string pathRNDSufix:   ""              // when changed the image is refreshed (updated)
    property var    symbolsModel:   []              // array of all configurable steps (Array<StepProps>)
    property var    topLevelModel:  []              // elements of the symbolsModel array with child-only steps filtered out

    property var    activeStep:             null    // holds the selected step
    property int    activeStepIndex:        -1      // holds the index of the selected step
    property bool   waitToRefreshStepImage: false   // used to force step image refresh (after a new image acquisition)

    property int    trainingInspectionID:      1;

    // signals
    // -------------------------------------------------------------
    signal trainImageReady()

    // event handling
    // -------------------------------------------------------------
    function getSteps(withUpdate) {
        var stepsJson = uiController.getConfigurableSteps(withUpdate);
        symbolsModel = JSON.parse(stepsJson);
        console.log(symbolsModel);
        // filter steps and add the original index as attribute
        var i = 0;
        topLevelModel = symbolsModel
            .map(function(element){
                var newElement = element;
                newElement.index = i++;
                return newElement;
            })
            .filter(function(element) {
                return element.childOnly === false;
            });

        listPanel.refresh();
    }

    function managerReady(requestor) {
        delayedExecution.acknowledge(requestor);
        container.enabled = true;
    }

    // .............................................................
    Component.onCompleted: {
        uiController.managerReady.connect(managerReady);
        uiController.stepImageReady.connect(trainImageReady);
        uiController.trainImageReady.connect(trainImageReady);
        uiController.setEditMode();

        uiController.stopTest(7);
        uiController.startTest(6);

        getSteps(false);
        imageInspect.roiArray = symbolsModel.filter( function(item) { return item.editRoi === true; } );
    }

    // .............................................................
    Component.onDestruction: {
        uiController.managerReady.disconnect(managerReady);
        uiController.trainImageReady.disconnect(trainImageReady);
        uiController.stepImageReady.disconnect(trainImageReady);

        if (waitingImage === true) {
            uiController.cancelAcquireImage();
        }

        uiController.startTest(7);
    }

    // .............................................................
    Timer {
        id: acquireStepImageDelay

        interval: 10 // 500
        repeat: false

        onTriggered: {
            var commingFromNewAcquisition = waitToRefreshStepImage;
            waitToRefreshStepImage = false;

            // tryout children first, if it's necessary
            if ( activeStep.tryChildren ) {

                delayedExecution.execute(
                    "imageRefresh",
                     (function (activeStep) {

                         uiController.tryOutSteps( JSON.stringify(activeStep), "imageRefresh" );
                         // uiController.regenerate( "imageRefresh" );

                         if (activeStep.useInputImage || activeStep.useOutputImage) {
                             // TODO: replace symName with path - check all functions in the call chain
                             uiController.acquireStepImage(activeStep.symName, activeStep.useOutputImage, activeStep.showGraphics);
                         }
                         else if ( !commingFromNewAcquisition && activeStep.showGraphics ) {
                             uiController.acquireRootImage( activeStep.showGraphics );
                         }

                     }).bind(this, activeStep),
                     true
                );
            }
            else {
                if (activeStep.useInputImage || activeStep.useOutputImage) {
                    // TODO: replace symName with path - check all functions in the call chain
                    uiController.acquireStepImage(activeStep.symName, activeStep.useOutputImage, activeStep.showGraphics);
                }
                else if ( !commingFromNewAcquisition && activeStep.showGraphics ) {
                    uiController.acquireRootImage( activeStep.showGraphics );
                }
            }

        }
    }

    // .............................................................
    onTrainImageReady: {
        if (waitToRefreshStepImage) {
            // we cannot call acquireImage/acquireStepImage - stack overflow
            // so we call acquireStepImage with delay
            acquireStepImageDelay.start();
            return;
        }

        waitingImage = false;
        pathRNDSufix = "-" + Math.random(10);
        imageInspect.source = "image://train" + trainingInspectionID + "/snapshot"  + pathRNDSufix;
    }

    // .............................................................
    function acquireAndRefreshStepImage( showGraphics ) {
        waitToRefreshStepImage = true;
        uiController.acquireImage( showGraphics );
    }

    // .............................................................
    function refreshStepImage() {
        acquireStepImageDelay.start();
    }

    ColumnLayout {

        anchors.fill: parent

        spacing: _uimButtonHeight * 0.2

        // page rendering
        // -------------------------------------------------------------
        FormHeader {
            Layout.fillWidth: true
            Layout.preferredHeight: _uimButtonHeight
            Layout.margins: 0
            text: "Edit Program  (" + configId + ")"
        }

        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: _uimButtonHeight * 0.2

            // IMAGES PANEL
            // ---------------------------------------------------------
            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredWidth: _uimButtonHeight * 0.6

                spacing: _uimButtonHeight * 0.1

                ImageInspect {
                    id: imageInspect

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredHeight: _uimHeight * 0.8

                    colorPick: false

                    property string colorValueText

                    source: "image://train" + trainingInspectionID + "/snapshot"  + pathRNDSufix;

                    BusyIndicator {
                        id: waitIndicator
                        width: _uimButtonHeight
                        height: _uimButtonHeight
                        anchors.centerIn: parent
                        running: waitingImage === true
                    }

                    onColorPicked: {
                        colorValueText = "H: " + hue + ", S: " + saturation + ", I:" + intensity;
                        console.log("hue: " + hue + ", saturation: " + saturation + ", intensity:" + intensity);
                    }
                }

                RowLayout {
                    height: _uimButtonHeight

                    Button {
                        id: requestInspection1
                        iconSource: trainingInspectionID == 1 ? "../images/cam1_active.png" : "../images/cam1.png"
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            waitingImage = true;
                            trainingInspectionID = 1;

                            if (activeStep && (activeStep.useInputImage || activeStep.useOutputImage) ) {
                                // acquire new image and when it comes require the step input image
                                acquireAndRefreshStepImage(activeStep && activeStep.showGraphics );
                            }
                            else {
                                uiController.acquireImage( activeStep && activeStep.showGraphics );
                            }

                            uiController.startTest(6);
                        }
                    }
                    Button {
                        id: requestInspection2
                        style: BlackButtonStyle {}
                        iconSource: trainingInspectionID == 2 ? "../images/cam2_active.png" : "../images/cam2.png"
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            waitingImage = true;
                            trainingInspectionID = 2;

                            if (activeStep && (activeStep.useInputImage || activeStep.useOutputImage) ) {
                                // acquire new image and when it comes require the step input image
                                acquireAndRefreshStepImage(activeStep && activeStep.showGraphics );
                            }
                            else {
                                uiController.acquireImage( activeStep && activeStep.showGraphics );
                            }

                            uiController.startTest(6);
                        }
                    }
                    Button {
                        id: requestInspection3
                        style: BlackButtonStyle {}
                        iconSource: trainingInspectionID == 3 ? "../images/cam3_active.png" : "../images/cam3.png"
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            waitingImage = true;
                            trainingInspectionID = 3;

                            if (activeStep && (activeStep.useInputImage || activeStep.useOutputImage) ) {
                                // acquire new image and when it comes require the step input image
                                acquireAndRefreshStepImage(activeStep && activeStep.showGraphics );
                            }
                            else {
                                uiController.acquireImage( activeStep && activeStep.showGraphics );
                            }

                            uiController.startTest(6);
                        }
                    }
                    Button {
                        id: requestInspection4
                        iconSource: trainingInspectionID == 4 ? "../images/cam4_active.png" : "../images/cam4.png"
                        style: BlackButtonStyle {}
                        Layout.preferredHeight: _uimButtonHeight

                        onClicked: {
                            waitingImage = true;
                            trainingInspectionID = 4;

                            if (activeStep && (activeStep.useInputImage || activeStep.useOutputImage) ) {
                                // acquire new image and when it comes require the step input image
                                acquireAndRefreshStepImage(activeStep && activeStep.showGraphics );
                            }
                            else {
                                uiController.acquireImage( activeStep && activeStep.showGraphics );
                            }

                            uiController.startTest(6);
                        }
                    }
                    Button {
                        id: zoomfit
                        iconSource: "../images/zoomfit.png"
                        style: BlackButtonStyle { }
                        Layout.leftMargin: 30
                        Layout.preferredHeight: _uimButtonHeight
                        onClicked: {
                            imageInspect.zoomToFit();
                        }
                    }
                    Button {
                        id: zoomin
                        iconSource: "../images/zoomin.png"
                        style: BlackButtonStyle { }
                        Layout.preferredHeight: _uimButtonHeight
                        onClicked: {
                            imageInspect.zoomIn();
                        }
                    }
                    Button {
                        id: zoomout
                        iconSource: "../images/zoomout.png"
                        style: BlackButtonStyle { }
                        Layout.preferredHeight: _uimButtonHeight
                        onClicked: {
                            imageInspect.zoomOut();
                        }
                    }

                }

                // action buttons
                RowLayout {
                    height: _uimButtonHeight
                    Layout.alignment: Qt.AlignBottom
                    Layout.topMargin: _uimButtonHeight * 0.2

                    spacing: _uimButtonHeight * 0.2

                    Button {
                        Layout.preferredHeight: _uimButtonHeight
                        style: BlackButtonStyle {}
                        text: "Close"

                        onClicked: {
                            stackView.pop();
                        }
                    }
                    Rectangle {
                        id: colorDisplay

                        Layout.preferredHeight: _uimButtonHeight
                        Layout.fillWidth: true

                        color: "transparent"

                        Text {
                            text: imageInspect.colorValueText

                            anchors.fill: parent

                            color: "#e7e7e7"
                            font.family: openSans.name
                            font.pixelSize: height * 0.25

                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

            }

            // EDIT PANEL
            // ---------------------------------------------------------
            Flipable {
                id: container

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredWidth: _uimButtonHeight * 0.4

                property bool flipped: false

                state: "front"

                transform: Rotation {
                        id: rotation
                        origin.x: container.width/2
                        origin.y: container.height/2
                        axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
                        angle: 0                            // the default angle
                }

                states: [
                    State {
                        name: "back"
                        PropertyChanges { target: rotation; angle: 180 }
                        PropertyChanges { target: editPanel; z: 100 }
                        PropertyChanges { target: listPanel; z: 0 }
                        when: container.flipped
                    },
                    State {
                        name: "front"
                        PropertyChanges { target: rotation; angle: 0 }
                        PropertyChanges { target: editPanel; z: 0 }
                        PropertyChanges { target: listPanel; z: 100 }
                        when: ! container.flipped
                    }
                ]

                transitions: Transition {
                    NumberAnimation { target: rotation; property: "angle"; duration: 500 }
                }

                // Step list
                // -------------------------------------------------------------
                front: StepListView {
                    id: listPanel

                    anchors.fill: parent
                    clip: true

                    stepsModel: topLevelModel

                    onStepSelected: {
                        if (container.flipped)
                            return;

                        editPanel.stepModel = step;
                        activeStep = step;
                        activeStepIndex = index;

                        // prepare the image inspect ROI vizualisations -------------------------------------
                        if (step.editRoi) {
                            var stepsToEdit = [];

                            if (step.showParent === true) {
                                stepsToEdit.push(symbolsModel[index]);      // push step
                                stepsToEdit.push(symbolsModel[index-1]);    // push parent step
                            }
                            else if (step.editGroup === true) {
                                stepsToEdit.push(step);

                                // append the children of the selected step
                                for (var i = index + 1; i < symbolsModel.length; i++) {
                                    if (symbolsModel[i].parentPath === step.path) {
                                        stepsToEdit.push(symbolsModel[i]);
                                    }
                                }
                            }
                            else {
                                stepsToEdit.push(symbolsModel[index]);      // push step
                            }

                            editPanel.positionModeActive = true;
                            editPanel.groupEditActive = false;
                            editPanel.rectTarget = imageInspect.setRoiArray(stepsToEdit);
                        }

                        if (step.useInputImage || step.useOutputImage || step.showGraphics) {
                            refreshStepImage();
                            // uiController.acquireStepImage(step.symName, step.useOutputImage, step.showGraphics);
                        }

                        editPanel.refresh();
                        container.flipped = true;

                        imageInspect.colorValueText = "";
                        imageInspect.colorPick = (activeStep.type == "Step.ThreshColor.1");
                    }
                }

                // Step Edit
                // -------------------------------------------------------------
                back: StepEditView {
                    id: editPanel

                    anchors.fill: parent

                    onClose: {
                        container.flipped = false;
                        rectTarget = undefined;

                        if (stepModel.useInputImage || stepModel.useOutputImage) {
                            // we are closing the edit of a step which uses an input image
                            // so we need to acquire a new image again
                            uiController.acquireRootImage();
                        }

                        // get the steps again
                        getSteps(true);

                        // update inspect-image control
                        imageInspect.editMode = false;
                        imageInspect.roiArray = symbolsModel.filter(function(item){return item.editRoi === true;});

                        // clear active step
                        activeStep = null;
                        listPanel.setActiveIndex(activeStepIndex);
                    }

                    onPositionModeActiveChanged: {
                        imageInspect.positionMode = positionModeActive;
                    }

                    onGroupEditActiveChanged: {
                        imageInspect.groupEdit = groupEditActive;
                    }

                    onTrainStep: {
                        delayedExecution.execute(
                            "train",
                            (function(stepPath) {
                                // train the step
                                uiController.train(stepPath);

                                // get the step data again to refresh the UI
                                var stepJson = uiController.getStep(stepPath);
                                stepModel = JSON.parse(stepJson);
                            })
                            .bind(this, stepPath)
                        );
                    }

                    onAcceptValue: {
                        delayedExecution.execute(
                            "acceptValue",
                            (function(stepPath, stepJson) {
                                container.enabled = false;

                                uiController.setDatumValue(stepPath, stepJson);

                                // get the step data again to refresh the UI
                                uiController.tryout(stepPath, "acceptValue");
                                var stepJsonSer = uiController.getStep(stepPath);
                                var newStep = JSON.parse(stepJsonSer);
                                stepModel.datums = newStep.datums;

                                // inform list that the model data have been changed
                                //stepDatumView.stepChanged();
                                editPanel.stepModelChanged();

                                // update image if necessary
                                if (newStep.useInputImage || newStep.useOutputImage || newStep.showGraphics) {
                                    // TODO: we must tryout child first if is necessary
                                    // uiController.tryOutSteps(JSON.stringify(newStep.symName));

                                    refreshStepImage();
                                    // uiController.acquireStepImage(newStep.symName, newStep.useOutputImage, newStep.showGraphics);
                                }

                            })
                            .bind(this, stepPath, stepJson),
                            true                                    // wait acknowledge
                        );
                    }

                    onDeleteStep: {
                        delayedExecution.execute(
                            "delete",
                            (function(stepPath) {
                                // delete the step
                                uiController.deleteStep(stepPath);
                                close();
                            })
                            .bind(this, stepPath)
                        );
                    }

                    onApplyShapes: {
                        delayedExecution.execute(
                            "applyShapes",
                            (function(stepPath) {
                                var updatedSteps = imageInspect.getROIs();
                                var stepsJSON = JSON.stringify(updatedSteps);

                                uiController.updateROIs(stepsJSON);

                                if (stepModel.autoTrainParent) {
                                    uiController.trainParent(stepPath);
                                }
                            })
                            .bind(this, stepPath)
                        );
                    }

                    onTryOutStep: {
                        delayedExecution.execute(
                            "tryout",
                            (function(stepPath) {
                                // tryout the step
                                uiController.tryout(stepPath, "tryOut");

                                // get the step data again to refresh the UI
                                var stepJson = uiController.getStep(stepPath);
                                stepModel = JSON.parse(stepJson);

                                // update the image if necessary
                                if (stepModel.useInputImage || stepModel.useOutputImage || stepModel.showGraphics) {
                                    refreshStepImage();
                                }

                            })
                            .bind(this, stepPath)
                        );
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
            stackView.pop();
        }
    }
}
