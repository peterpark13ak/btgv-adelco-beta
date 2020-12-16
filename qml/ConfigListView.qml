import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import com.darebit.blisterocv 1.0

Item {
    id: batchConfigList

    // properties
    // -------------------------------------------------------------
    property var    configList:     uiController.getConfigurationNames()        // get list of configurations as string list
    property var    readOnlyConfig: uiController.getReadOnlyConfigurations()    // get list of read only configurations
    property string activeConfig:   activeConfigId                              // the curent active configuration

    // visual properties
    // -------------------------------------------------------------
    width:  parent.width
    height: parent.height

    // visual properties
    // -------------------------------------------------------------
    Component.onDestruction: {
        // reload the active AVP on exit (if necessary)
        delayedExecution.execute(
            "destruction",
            (function(activeConfigId) {
                uiController.loadAvp(activeConfigId);
            })
            .bind(this, activeConfigId)
        );
    }

    // Batch Configuration Edit page
    // -------------------------------------------------------------
    property Component batchConfigView: BatchConfigurationView {

        onScreenReady: {
            delayedExecution.acknowledge("batchConfig");
        }

        Component.onDestruction: {
            if (Stack.status === Stack.Inactive) {
                list.model = uiController.getConfigurationNames();
                list.enabled = true

                // mainWindow.statusJobReady = true;
            }
        }
    }

    ColumnLayout {

        anchors.fill: parent

        // Header
        // -------------------------------------------------------------
        FormHeader {
            Layout.preferredHeight: _uimButtonHeight
            Layout.fillWidth: true
            Layout.bottomMargin: _uimButtonHeight * 0.2
            text: "Batch Configurations"
        }

        // configuration list item rendering
        // -------------------------------------------------------------
        Component {
            id: configDelegate

            Rectangle {
                height: _uimButtonHeight
                width: parent.width
                color: readOnlyConfig.indexOf(modelData) >= 0 ? "#660000" : "#336699"
                opacity: mouseHandler.pressed || ! list.enabled ? 0.3 : 1

                Text {
                    anchors.fill: parent
                    anchors.margins: _uimButtonHeight * 0.4
                    color: "#ffffff"
                    font.pixelSize: _uimFontSize
                    verticalAlignment: Text.AlignVCenter

                    text: modelData + (activeConfig === modelData ? " (active)" : "")
                }

                MouseArea {
                    id: mouseHandler
                    anchors.fill: parent

                    onClicked: {
//                        mainWindow.statusJobReady = false;
                        list.enabled = false

//                        delaySelectConfiguration.modelData = modelData;
//                        delaySelectConfiguration.start();

                        delayedExecution.execute(
                            "batchConfig",
                            (function(modelData) {
                                stackView.push({item: batchConfigView, properties: { configId: modelData } });
                                stackView.currentItem.forceActiveFocus();
                            })
                            .bind(this, modelData),
                            true
                        );
//                        stackView.push({item: batchConfigView, properties: { configId: modelData } });
//                        stackView.currentItem.forceActiveFocus();
                    }
                }
            }
        }

        // List of configurations
        // -------------------------------------------------------------
        ListView {
            id: list

            clip: true
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: _uimButtonHeight * 0.2

            model: configList
            delegate: configDelegate
        }

        // form action buttons
        // -------------------------------------------------------------
        RowLayout {
            spacing: _uimButtonHeight * 0.2

            Button {
                Layout.preferredHeight: _uimButtonHeight
                style: BlackButtonStyle {}
                text: "Close"

                onClicked: {
                    // mainWindow.statusJobReady = false;
                    stackView.pop();
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
