import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Item {
    id: editPanel

    // properties
    // -------------------------------------------------------------
    property var    stepModel: ({})
    property var    rectTarget
    property bool   positionModeActive // : true
    property bool   groupEditActive: false
    property bool   roiEdit: false

    // signals
    // -------------------------------------------------------------
    signal close()
    signal cancel()

    signal deleteStep(string stepPath)
    signal tryOutStep(string stepPath)
    signal trainStep(string stepPath)
    signal applyShapes(string stepPath)
    signal acceptValue(string stepPath, string stepJson)

    // event handling
    // -------------------------------------------------------------
    onStepModelChanged: {
        if (    stepModel.editRoi === false ||
                stepModel.roiValue === undefined ||
                stepModel.roiValue[0] === 0 &&
                stepModel.roiValue[1] === 0 &&
                stepModel.roiValue[2] === 0 &&
                stepModel.roiValue[3] === 0 ) {
            roiEdit = false;
        }
        else {
            roiEdit = true;
        }
    }

    function refresh() {
        stepEditPages.currentIndex = 0;
    }

    onGroupEditActiveChanged: {
        positionModeActive = true;
    }

    // rendering
    // -------------------------------------------------------------
    ColumnLayout {
        anchors.fill: parent
        spacing: _uimButtonHeight * 0.2

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Header ------------------------------------------------------
            Rectangle {
                id: headWrapper

                width: parent.width - _uimButtonHeight * 2.2
                height: _uimButtonHeight
                color: "#336699"

                Text {
                    text: (editPanel.stepModel["name"] || "")
                    color: "#ffffff"
                    font.pixelSize: _uimFontSize
                    x: _uimButtonHeight * 0.4
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            TabView {
                id: stepEditPages

                anchors.fill: parent

                style: TouchTabViewStyle {
                    tabWidth: _uimButtonHeight * 1.2
                    tabHeight: _uimButtonHeight
                    tabsAlignment: Qt.AlignRight
                }

                Tab {
                    title: "Place"

                    anchors.fill: parent

                    StepPlacementView {
                        anchors.fill: parent
                    }
                }

                Tab {
                    id: propsTab

                    title: "Props"
                    enabled: stepModel["datums"] !== undefined ? (stepModel.datums.length > 0) : false

                    StepDatumsView {
                        id: stepDatumsView

                        step: stepModel

                        anchors.fill: parent
                        anchors.topMargin: _uimButtonHeight * 0.2

                        onAcceptedValue: {
                            acceptValue(stepPath, stepJson);
                        }
                    }
                }
            }
        }

        // Action Buttons
        // -------------------------------------------------------------------------------------------
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: _uimButtonHeight

            spacing: _uimButtonHeight * 0.1

            // Apply ROI changes
            // --------------------------------------------------------------------------------------
            Button {
                enabled: roiEdit

                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                text: "Apply"

                onClicked: {
                    applyShapes(stepModel.path);
                }
            }

            // Train
            // --------------------------------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                Layout.preferredWidth: _uimButtonHeight
                style: BlackButtonStyle { margin: _uimButtonHeight * 0.1 }
                text: "Train"
                enabled: ! (stepModel.trained === true)
                visible: false // stepModel.trainable === true

                onClicked: {
                    trainStep(stepModel.path);
                }
            }

            // Tryout
            // --------------------------------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                text: "Try"

                onClicked: {
                    tryOutStep(stepModel.path);
                }
            }

            // Delete
            // --------------------------------------------------------------------------------------
            Button {
                enabled: stepModel.allowDelete === true

                Layout.preferredHeight: _uimButtonHeight
                Layout.alignment: Qt.AlignRight
                style: BlackButtonStyle {}
                text: "Delete"

                onClicked: {
                    deleteStep(stepModel.path);
                }
            }

            // Cancel
            // --------------------------------------------------------------------------------------
            Button {
                Layout.preferredHeight: _uimButtonHeight
                Layout.alignment: Qt.AlignRight
                style: BlackButtonStyle {}
                text: "Back"

                onClicked: {
                    close();
                }
            }
        }
    }
}
