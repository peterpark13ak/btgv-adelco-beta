/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

ButtonStyle {
    property color fontColor: (control.enabled ? "#e7e7e7" : "#606060")

    property int bWidth
    property string fontFamily
    property int fontSize
    property url rightAlignedIconSource
    property int margin: control.text.length === 0 ? 0 : textSingleton.implicitHeight;
    property string pressColor: "#202020"
    property string releaseColor: "#333"

    background: BlackButtonBackground {
        pressed: control.pressed // || (control.checked || false)
        pressedColor: pressColor
        releasedColor: releaseColor
        opacity: control.pressed ? 0.3 : 1
    }

    label: Item {
        implicitWidth: row.implicitWidth + 2 * margin
        implicitHeight: row.implicitHeight
        baselineOffset: row.y + text.y + text.baselineOffset

        Row {
            id: row
            anchors.left: control.text.length === 0 ? undefined : parent.left
            anchors.leftMargin: (control.text.length !== 0 && ! (""+control.iconSource)) ? (margin || textSingleton.implicitHeight) : 0
            anchors.rightMargin: control.text.length === 0 ? 0 : textSingleton.implicitHeight
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: control.text.length === 0 ? parent.horizontalCenter : undefined
            spacing: textSingleton.implicitHeight

            Image {
                source: control.iconSource
                width: sourceSize.width ? height:  0 // sourceSize.width // Math.min(sourceSize.width, height)
                height: control.height * 0.9 // 2 * text.height // sourceSize.height // text.implicitHeight
                fillMode: Image.PreserveAspectFit
                opacity: control.enabled ? 1.0 : 0.4
            }
            Text {
                id: text
                text: control.text
                width: bWidth ? bWidth - 2*margin : undefined;
                color: fontColor
                opacity: control.pressed ? 0.3 : 1
                font.pixelSize: fontSize ? fontSize : control.height * 0.25
                font.family: fontFamily.length > 0 ? fontFamily : openSans.name;
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Loader {
            active: rightAlignedIconSource.toString().length !== 0
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: textSingleton.implicitHeight

            sourceComponent: Image {
                width: Math.min(sourceSize.width, height)
                height: text.implicitHeight
                fillMode: Image.PreserveAspectFit
                source: rightAlignedIconSource
            }
        }
    }
}
