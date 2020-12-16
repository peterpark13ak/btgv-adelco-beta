import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle {
    id: imageInspect

    // properties
    // -------------------------------------------------------------
    property string source                  // the image source
    property double viewScale: 1.0          // view scale
    property var    roiArray: []            // array with ROIs
    property int    editIndex               // the index of the main editable ROI
    property int    childIndex              // the currently selected child index (all children after this are modified)
    property bool   groupMode               // group mode when we edit a step along with children

    property var    trackedRoi: undefined
    property bool   positionMode: true      // if true the position is changed. If false the size
    property bool   editMode: false         // when in edit mode use local coordinates
    property bool   groupEdit: false        // if true the children of the editIndex-roi are modified
    property bool   colorPick: false        // when true, color picking signal is emmited on click

    // visual properties
    // -------------------------------------------------------------
    color:          "#303030"
    border.color:   "#666666"
    border.width:   1

    // signals
    // -------------------------------------------------------------
    signal nextChild();
    signal alignTop();
    signal sameSize();
    signal distribute();
    signal colorPicked(int hue, int saturation, int intensity);

    // event handling
    // -------------------------------------------------------------
    onViewScaleChanged: {
        mouseTracker.adjustScale();
    }

    onGroupEditChanged: {
        childIndex = 1;
        if (groupEdit === true) {
            setEditROI(childIndex);
        }
        else {
            setEditROI(0);
        }
    }

    onNextChild: {
        // get the next child Index
        childIndex = (childIndex + 1) % roiArray.length;
        childIndex = childIndex === 0 ? 1 : childIndex;

        setEditROI(childIndex);
    }

    onAlignTop: {
        var refY = roiRectangles.itemAt(childIndex).y;

        for (var i = childIndex + 1; i < roiArray.length; i++) {
            roiRectangles.itemAt(i).y = refY;
        }
        mouseTracker.startTrack();
    }

    onSameSize: {
        var refWidth = roiRectangles.itemAt(childIndex).width;
        var refHeight = roiRectangles.itemAt(childIndex).height;

        for (var i = childIndex + 1; i < roiArray.length; i++) {
            roiRectangles.itemAt(i).width = refWidth;
            roiRectangles.itemAt(i).height = refHeight;
        }
        mouseTracker.startTrack();
    }

    onDistribute: {
        var lastIndex = roiArray.length - 1;
        var distance = roiRectangles.itemAt(lastIndex).x - roiRectangles.itemAt(childIndex).x;
        var steps = lastIndex - childIndex;
        var step = distance /steps;
        var startx = roiRectangles.itemAt(childIndex).x;

        for (var i = 1; i < steps + 1; i++ ) {
            roiRectangles.itemAt(childIndex + i).x = startx + i * step;
        }
        mouseTracker.startTrack();
    }

    // functions
    // -------------------------------------------------------------
    function setRoiArray(roiEntries) {
        childIndex = 1;
        editMode = true;
        roiArray = roiEntries;
        groupMode = roiArray[0].editGroup === true;
        zoomTarget(roiArray[0].roiValue);

        return setEditROI(0);
    }

    function zoomToFit() {
        viewScale = imageWrapper.width / image.sourceSize.width;
    }

    function zoomIn() {
        viewScale = Math.min(viewScale + 0.10, 2);
        centerAt(roiArray[0].roiValue);
    }

    function zoomOut() {
        var minScale = imageWrapper.width / image.sourceSize.width;
        viewScale = Math.max(viewScale - 0.10, 0.01);

        if (viewScale < minScale) {
            zoomToFit();
        }
        else {
            centerAt(roiArray[0].roiValue);
        }
    }

    function zoomTarget(roiValue) {
        var minScale = imageWrapper.width / image.sourceSize.width;
        viewScale = Math.min(1.0, Math.max(0.1, 0.8 * imageWrapper.width / Math.max(roiValue[2], 1)));

        if (viewScale < minScale) {
            zoomToFit();
        }
        else {
            centerAt(roiValue);
        }
    }

    function centerAt(roiValue) {
        var xpos = roiValue[0] + roiValue[2]/2.0;
        var ypos = roiValue[1] + roiValue[3]/2.0;

        imageWrapper.contentX = viewScale * xpos - imageWrapper.width / 2.0;
        imageWrapper.contentY = viewScale * ypos - imageWrapper.height / 2.0;
    }

    function setEditROI(index) {
        trackedRoi = roiRectangles.itemAt(index);
        highlightRects(index);

        mouseTracker.startTrack();

        return mouseTracker;
    }

    function highlightRects(index) {
        roiRectangles.itemAt(0).edit = index === 0 ? true : false;

        for (var i = 1; i < roiArray.length; i++) {
            roiRectangles.itemAt(i).edit = i >= index && index !== 0 ? true : false;
        }
    }

    function getROI() {
        var roi = [
                    trackedRoi.x/viewScale,
                    trackedRoi.y/viewScale,
                    trackedRoi.width/viewScale,
                    trackedRoi.height/viewScale
                ];

        return roi;
    }

    function getROIs() {
        var refx = roiRectangles.itemAt(0).x;
        var refy = roiRectangles.itemAt(0).y;

        roiArray[0].roiValue[0] = roiRectangles.itemAt(0).x/viewScale;
        roiArray[0].roiValue[1] = roiRectangles.itemAt(0).y/viewScale;
        roiArray[0].roiValue[2] = roiRectangles.itemAt(0).width/viewScale;
        roiArray[0].roiValue[3] = roiRectangles.itemAt(0).height/viewScale;

        for (var i = 1; groupMode && (i < roiArray.length); i++) {
            var refToParent = roiArray[i].relativeToParent;

            roiArray[i].roiValue[0] = (roiRectangles.itemAt(i).x - (refToParent ? refx : 0))/viewScale;
            roiArray[i].roiValue[1] = (roiRectangles.itemAt(i).y - (refToParent ? refy : 0))/viewScale;
            roiArray[i].roiValue[2] = roiRectangles.itemAt(i).width/viewScale;
            roiArray[i].roiValue[3] = roiRectangles.itemAt(i).height/viewScale;
        }
        return roiArray;
    }

    // Rendering
    // -------------------------------------------------------------
    Flickable {
        id: imageWrapper

        // visual properties
        // ---------------------------------------------------------
        anchors { fill: parent; margins: 4 }
        contentWidth: imageFrame.width
        contentHeight: imageFrame.height
        clip: true

        Rectangle {
            id: imageFrame

            width: image.width
            height: image.height

            Image {
                id: image
                x: 0; y: 0
                width: sourceSize.width * viewScale
                height: sourceSize.height * viewScale

                cache: false
                asynchronous: false

                source: imageInspect.source
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (colorPick) {
                            var colorJson = uiController.getImageHSI((mouse.x / viewScale), (mouse.y / viewScale));
                            var color = JSON.parse(colorJson);
                            colorPicked(color.hue, color.saturation, color.intensity);
                        }

                    }
                }
            }

            Rectangle {
                id: mouseTracker

                color: "transparent"  // "#80ff0000"

                property bool editPosition: positionMode
                property int refX
                property int refY
                property int refWidth
                property int refHeight
                property double refScale: 0.0
                property bool adjasting: false;

                property var childrenReference: [];

                signal startTrack()
                signal stopTrack()

                onEditPositionChanged: {
                    if (!editMode) return;
                    startTrack();
                }

                onStartTrack: {
                    if (groupEdit === true) {
                        var parentRect = roiRectangles.itemAt(0);
                        var refRect = roiRectangles.itemAt(childIndex);  //(1);

                        childrenReference = [];
                        // for (var i = 1; i < roiArray.length; i++) {
                        for (var i = childIndex; i < roiArray.length; i++) {
                            var rect = roiRectangles.itemAt(i);
                            childrenReference.push({
                               refX: rect.x - refRect.x,
                               refY: rect.y - refRect.y,
                               refWidth: rect.width,
                               refHeight: rect.height
                            });
                        }
                    }

                    x = trackedRoi.x;
                    y = trackedRoi.y
                    refX = x;
                    refY = y;
                    refWidth = trackedRoi.width;
                    refHeight = trackedRoi.height;
                    refScale = viewScale;

                    // just for test !!!!!!!!!! --------------------
                    width = trackedRoi.width;
                    height = trackedRoi.height;
                    // ---------------------------------------------
                }

                function adjustScale() {
                    if (refScale === 0.0 || trackedRoi === undefined || trackedRoi === null)
                        return;

                    adjasting = true;
                    // ------------------------------------------------------
                    for (var i = 0; i < roiArray.length; i++) {
                        var rect = roiRectangles.itemAt(i);
                        rect.x = (rect.x/refScale) * viewScale;
                        rect.y = (rect.y/refScale) * viewScale;
                        rect.width = rect.width/refScale * viewScale;
                        rect.height = rect.height/refScale * viewScale;
                    }
                    startTrack();
                    // ------------------------------------------------------
                    adjasting = false;
                }

                onXChanged: {
                    if (adjasting === true)
                        return;

                    if (groupEdit === true) {
                        if (editPosition) {
                            for (var i = childIndex; i < roiArray.length; i++) {
                                roiRectangles.itemAt(i).x = childrenReference[i-childIndex].refX + x;
                            }
                        }
                        else {
                            for (var j = childIndex; j < roiArray.length; j++) {
                                roiRectangles.itemAt(j).width = Math.max(
                                            childrenReference[j-childIndex].refWidth + (x - refX),
                                            2);
                            }
                        }
                    }
                    else {
                        if (editPosition) {
                            trackedRoi.x = x;
                        } else {
                            trackedRoi.width = Math.max(refWidth + (x - refX), 2);
                            width = trackedRoi.width;
                        }
                    }
                }

                onYChanged: {
                    if (adjasting === true)
                        return;

                    if (groupEdit === true) {
                        if (editPosition) {
                            for (var i = childIndex; i < roiArray.length; i++) {
                                roiRectangles.itemAt(i).y = childrenReference[i-childIndex].refY + y;
                            }
                        }
                        else {
                            for (var j = childIndex; j < roiArray.length; j++) {
                                roiRectangles.itemAt(j).height = Math.max(
                                            childrenReference[j-childIndex].refHeight + (y - refY),
                                            2);
                            }
                        }
                    }
                    else {
                        if (editPosition) {
                            trackedRoi.y = y;
                        } else {
                            trackedRoi.height = Math.max(refHeight + (y - refY), 2);
                            height = trackedRoi.height;
                        }
                    }
                }

            }

            Repeater {
                id: roiRectangles
                model: roiArray
                delegate: Rectangle {
                    id: canvas

                    property string label: modelData.name
                    property bool edit // : editMode && index === 0
                    property bool relativeToParent: modelData.relativeToParent              // the step has relatine-to-parent coordinates and ...
                                                    && ( ! editMode                         // we are not in edit mode
                                                        || editMode && groupMode === true   // or we are in edit-group mode
                                                        )

                    x: ((relativeToParent ? modelData.parentX : 0) + modelData.roiValue[0]) * viewScale
                    y: ((relativeToParent ? modelData.parentY : 0) + modelData.roiValue[1]) * viewScale
                    width: modelData.roiValue[2] * viewScale
                    height: modelData.roiValue[3] * viewScale
                    border.width: 1
                    border.color: edit ? "#66FF66" :"#66AAFF"
                    color: edit ? "#44993333" : "transparent"

                    Text {
                        text: label
                        visible: ! editMode
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        font.pixelSize: 9
                        color: "white"
                    }
                }
            }
        }
    }
}
