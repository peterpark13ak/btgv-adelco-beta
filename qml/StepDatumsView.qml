import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

Item {

    id: stepDatumView

    // **********************************************************************************
    // The StepDatumsView component is depended on the following:
    //   1. the batchConfiguration in the ancestor path as a stackview container
    // **********************************************************************************

    property var step

    signal acceptedValue(string stepPath, string stepJson)

    // rendering
    // ---------------------------------------------------------------

    // visual numeric keyboard
    // ---------------------------------------------------------------
    property Component numericKeypad: NumericKeypadView {}

    function editNumber(label, target) {
        batchConfiguration.Stack.view.push({
            item: numericKeypad,
            properties: {
                title: label,
                target: target }
        });
    }

    // The datums list
    // ---------------------------------------------------------------
    ListView {
        id: datumsList

        anchors.fill: parent
        spacing: _uimButtonHeight * 0.2
        clip: true

        model: step.datums

        delegate:  Item {

            width: parent.width
            height: modelData.readOnly === true ? _uimButtonHeight * 0.2 : _uimButtonHeight * 1.3

            NumberInputSpinner {
                id: numericDatum

                visible:    modelData.type === 1    // integerType

                label:      modelData.label + ":"
                minValue:   modelData.minValue
                maxValue:   modelData.maxValue
                value:      modelData.value
                readOnly:   modelData.readOnly

                function updateValue() {
                    value = modelData.value;
                }

                Component.onCompleted: {
                    datumsList.onModelChanged.connect(updateValue);
                }

                Component.onDestruction: {
                    datumsList.onModelChanged.disconnect(updateValue);
                }

                onSelected: editNumber(modelData.label, numericDatum)

                onAccepted: {
                    step.datums[index].value = value;
                    acceptedValue(step.path, JSON.stringify(step.datums[index]));
                }
            }


            BooleanInput {
                id: booleanDatum

                visible:    modelData.type === 5    // booleanType

                label:      modelData.label + ":"
                value:      modelData.value
                readOnly:   modelData.readOnly

                function updateValue() {
                    value = modelData.value;
                }

                Component.onCompleted: {
                    datumsList.onModelChanged.connect(updateValue);
                }

                Component.onDestruction: {
                    datumsList.onModelChanged.disconnect(updateValue);
                }

                onAccepted: {
                    step.datums[index].value = value;
                    acceptedValue(step.path, JSON.stringify(step.datums[index]));
                }
            }
        }

    }
}
