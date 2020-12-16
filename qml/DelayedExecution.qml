import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    id: delayedExecution

    // Visual Properties
    // ---------------------------------------------------------------------
    anchors.fill: parent

    // Properties
    // ---------------------------------------------------------------------
    property var actions: ({})  // map to keep the delayed actions

    // Signals
    // ---------------------------------------------------------------------
    signal finished(string actionName)
    signal cancel(string actionName)

    // Event Handling
    // ---------------------------------------------------------------------
    onFinished: {
        var action = actions[actionName];
        if (!action)
            return;

        if (action.waitAck === true)
            return;

        acknowledge(actionName);
    }

    // ---------------------------------------------------------------------
    function execute(actionName, action, waitAck) {
        actions[actionName] = {
            callBack: action,
            waitAck: (waitAck === true)
        };

        // id:delayedAction;
        // delayedAction.destroy();

        var timerObject = actions[actionName].timer = Qt.createQmlObject(
            'import QtQuick 2.5; Timer { \

                property var action; \
                property string actionName; \
                interval: 10; \
                triggeredOnStart: false; \
                repeat: false; \

                onTriggered: { \
                    action(); \
                    parent.finished(actionName); \
                    this.destroy(); \
                } \
             }',
            delayedExecution
        );

        timerObject.action = action;
        timerObject.actionName = actionName;
        actions[actionName].timer = timerObject;

        busyIndicator.busy = true;

        console.log(">> Wait for " + actionName);
        timerObject.start();
    }

    // Acknowledge action
    // ---------------------------------------------------------------------
    function acknowledge(actionName) {

        delete actions[actionName];

        if (Object.keys(actions).length === 0) {
            busyIndicator.busy = false;
        }
    }

    // The Busy Indicator
    // ---------------------------------------------------------------------
    BusyScreen {
        id: busyIndicator

        onCancel: {
            if (Object.keys(actions).length === 0)
                return;

            var actionName = actions[Object.keys(actions)[0]].actionName;
            delayedExecution.cancel(actionName);
        }
    }
}
