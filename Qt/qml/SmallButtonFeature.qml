// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Button {
    id: button
    //checkable: true
    font.pixelSize: fontSizeExtraSmall
    leftPadding: 10
    rightPadding: 10
    topPadding: 10
    bottomPadding: 12
    implicitWidth: 70
    implicitHeight: 80

    icon.name: "placeholder"
    icon.width: 44
    icon.height: 44
    display: Button.TextUnderIcon
}
