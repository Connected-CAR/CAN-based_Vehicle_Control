// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Button {
    id: button
    //checkable: true
    font.pixelSize: 15
    leftPadding: 10
    rightPadding: 10
    topPadding: 10
    bottomPadding: 12
    implicitWidth: 50
    implicitHeight: 30

    icon.name: "placeholder"
    display: Button.TextUnderIcon
}
