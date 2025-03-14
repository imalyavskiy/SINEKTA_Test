/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.3
import TestApp.Data 1.0

ApplicationWindow {
  id: applicationWindow

  width: Screen.width * 0.7
  height: Screen.height * 0.7
  minimumWidth: width
  minimumHeight: height
  maximumWidth: minimumWidth
  maximumHeight: minimumHeight

  visible: true

  property TouchstoneData dataSource : TouchstoneData
  {
    onFileLoadSuccess: {
      statusText.color = "green"
      statusText.text = "[ SUCCESS ] File \"" + filePath + "\" load suceeded."
      console.log(statusText.text)

      touchstoneCanvas.requestPaint()
    }

    onFileLoadFailure: {
      statusText.color = "red"
      statusText.text = "[ FAILURE ] File \"" + filePath + "\" load failed.(" + reason +")"
      console.log(statusText.text)
    }
  }

  property FileDialog openDataFileDialog: FileDialog{
    id: fileDialog
    title: "Please choose a file"
    folder: shortcuts.home
    onAccepted: {
      console.log("You chose: " + fileDialog.fileUrls)
      dataSource.loadData(fileDialog.fileUrls, touchstoneCanvas.width, touchstoneCanvas.height, 10)
    }
    onRejected: {
        console.log("Canceled")
    }
    Component.onCompleted: visible = false
  }

  header: ToolBar {
    RowLayout {
      anchors.fill: parent
      ToolButton {
        anchors.fill: parent
        text: qsTr("Open Data File")
        onClicked: {
          console.log("\"Open Data File\" button pressed.")
          fileDialog.open();
        }
      }
    }
  }

  Canvas {
    id: touchstoneCanvas
    anchors.fill: parent

    property var color_value: ["#aa0000", "#00aa00", "#0000aa", "#00aaaa", "#aa00aa", "#aaaa00"]
    property int color_index: 0

    onPaint: {
      var ctx = getContext("2d")
      ctx.reset();

      var size = dataSource.size();
      console.log("dataSource.size() returned " + size);

      if(size > 0) {
        ctx.beginPath();

        ctx.lineWidth = 2;
        ctx.strokeStyle = color_value[color_index]

        var pt0 = dataSource.getPoint(0);
        ctx.moveTo(pt0.x, pt0.y);
        for(var i = 1; i < size; i++ )
        {
          var ptN = dataSource.getPoint(i);
          ctx.lineTo(ptN.x, ptN.y);
        }
        ctx.stroke();

        color_index = (color_index + 1) % 6
      }
    }
  }

  footer: Text
  {
    id: statusText
    text: "[ WAITING ] No file opened."
  }
}

