import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Imagine
import QtQuick.Window

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    minimumWidth: 1180
    minimumHeight: 663
    visible: true
    title: "Qt Quick Controls - Imagine Style Example: Automotive"

    readonly property color colorGlow: "#1d6d64"
    readonly property color colorWarning: "#d5232f"
    readonly property color colorMain: "#6affcd"
    readonly property color colorBright: "#ffffff"
    readonly property color colorLightGrey: "#888"
    readonly property color colorDarkGrey: "#333"

    readonly property int fontSizeExtraSmall: Qt.application.font.pixelSize * 0.8
    readonly property int fontSizeMedium: Qt.application.font.pixelSize * 1.5
    readonly property int fontSizeLarge: Qt.application.font.pixelSize * 2
    readonly property int fontSizeExtraLarge: Qt.application.font.pixelSize * 5

    Component.onCompleted: {
        x = Screen.width / 2 - width / 2
        y = Screen.height / 2 - height / 2
        DatabaseManager.startPolling();
        console.log("Uploading is Completed");

    }

    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    Frame {
        id: frame
        anchors.fill: parent
        anchors.margins: 90

        RowLayout {
            id: mainRowLayout
            anchors.fill: parent
            anchors.margins: 24

            Rectangle {
                color: colorMain
                implicitWidth: 1
                //Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.preferredWidth: 350
                Layout.fillWidth: true
                Layout.fillHeight: true

                LargeLabel {
                    id: timeLabel
                    font.pixelSize: fontSizeExtraLarge
                    Layout.alignment: Qt.AlignHCenter

                    // 타이머를 사용하여 주기적으로 시간 업데이트
                    Timer {
                        interval: 1000; running: true; repeat: true
                        onTriggered: {
                            var currentTime = new Date();
                            var hours = currentTime.getHours();
                            var minutes = currentTime.getMinutes();

                            // 시간 형식 설정 (12시간제로 변환)
                            var meridiem = (hours >= 12) ? "PM" : "AM";
                            hours = (hours > 12) ? hours - 12 : hours;
                            hours = (hours == 0) ? 12 : hours;

                            // 시간을 두 자리 숫자로 표시
                            hours = ("0" + hours).slice(-2);
                            minutes = ("0" + minutes).slice(-2);

                            // 시간과 분을 timeLabel에 대입
                            timeLabel.text = hours + ":" + minutes;

                            // AM/PM 구분을 largeLabelAM에 대입
                            largeLabelAM.text = meridiem;
                        }
                    }

                    LargeLabel {
                        id: largeLabelAM
                        font.pixelSize: fontSizeLarge
                        anchors.left: timeLabel.right
                        anchors.leftMargin: 8
                    }
                }



                Label {
                    id: dateLabel
                    color: "lightgrey"
                    font.pixelSize: 24

                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 2
                    Layout.bottomMargin: 10

                    // 타이머를 사용하여 주기적으로 시간 업데이트
                    Timer {
                        interval: 1000; running: true; repeat: true
                        onTriggered: {
                            var currentDate = new Date();
                            var year = currentDate.getFullYear();
                            // 월은 0부터 시작하므로 +1 해줘야 함
                            var month = currentDate.getMonth() + 1;
                            var day = currentDate.getDate();

                            // 월과 일을 두 자리 숫자로 표시
                            month = ("0" + month).slice(-2);
                            day = ("0" + day).slice(-2);

                            // 년도, 월, 일을 Label에 표시
                            dateLabel.text = year + "/" + month + "/" + day;
                        }
                    }
                }
                Image {
                    Layout.leftMargin: 50
                    source: "qrc:/icons/car.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                }

            }


            Rectangle {
                color: colorMain
                implicitWidth: 1
                Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.fillHeight: true
                spacing: 20

                RowLayout{
                    Layout.leftMargin: 50
                    spacing: 50
                    SmallButtonFeature{
                        icon.source: "qrc:/icons/automotive/44x44/left.png"
                        text: qsTr("Left")
                        font.pixelSize: Qt.application.font.pixelSize * 1.2
                    }

                    SmallButtonFeature{
                        icon.source: "qrc:/icons/automotive/44x44/wiper3.png"
                        text: qsTr("Wiper")
                        font.pixelSize: Qt.application.font.pixelSize * 1.2
                    }
                    SmallButtonFeature{
                        icon.source: "qrc:/icons/automotive/44x44/emer.png"
                        text: qsTr("Emer")
                        font.pixelSize: Qt.application.font.pixelSize * 1.2
                    }
                    SmallButtonFeature{
                        icon.source: "qrc:/icons/automotive/44x44/high.png"
                        icon.height: 43
                        topPadding: 17
                        text: qsTr("High")
                        font.pixelSize: Qt.application.font.pixelSize * 1.2
                    }
                    SmallButtonFeature{
                        icon.source: "qrc:/icons/automotive/44x44/right.png"
                        icon.width: 43
                        text: qsTr("Right")
                        font.pixelSize: Qt.application.font.pixelSize * 1.2
                    }
                }

                RowLayout{
                    Layout.leftMargin: 30
                    spacing: 10
                    OnOffFeature{
                        text: qsTr("On")
                        onClicked: DatabaseManager.turn_on_left();
                    }
                    OnOffFeature{
                        text: qsTr("Off")
                        onClicked: DatabaseManager.turn_off_left();
                    }
                    OnOffFeature{
                        text: qsTr("On")
                        onClicked: DatabaseManager.turn_on_wiper();
                    }
                    OnOffFeature{
                        text: qsTr("Off")
                        onClicked: DatabaseManager.turn_off_wiper();
                    }
                    OnOffFeature{
                        text: qsTr("On")
                        onClicked: DatabaseManager.turn_on_emer();
                    }
                    OnOffFeature{
                        text: qsTr("Off")
                        onClicked: DatabaseManager.turn_off_emer();
                    }
                    OnOffFeature{
                        text: qsTr("On")
                        onClicked: DatabaseManager.turn_on_high();
                    }
                    OnOffFeature{
                        text: qsTr("Off")
                        onClicked: DatabaseManager.turn_off_high();
                    }
                    OnOffFeature{
                        text: qsTr("On")
                        onClicked: DatabaseManager.turn_on_right();
                    }
                    OnOffFeature{
                        text: qsTr("Off")
                        onClicked: DatabaseManager.turn_off_right();
                    }
                }
                RowLayout{
                    Layout.leftMargin: 50
                    Label {
                        id: sensingLabel
                        color: colorLightGrey
                        font.pixelSize: fontSizeMedium

                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 2
                        Layout.bottomMargin: 10

                        Component.onCompleted: {
                            DatabaseManager.sensingChanged.connect(onSensingChanged)
                        }
                        function onSensingChanged(text) {
                            sensingLabel.text=text;
                        }
                    }
                }

                RowLayout{

                    Button {
                        id: button
                        //checkable: true
                        font.pixelSize: 15
                        leftPadding: 10
                        rightPadding: 10
                        topPadding: 10
                        bottomPadding: 12
                        implicitWidth: 70
                        implicitHeight: 120
                        Layout.leftMargin: 60
                        text: qsTr("Reset\n Rotation")
                        onClicked: DatabaseManager.mid();
                        icon.name: "placeholder"
                        display: Button.TextUnderIcon
                    }
                    BigButtonFeature {
                        Layout.leftMargin: 140
                        icon.source: "qrc:/icons/dir-button.png"
                        onClicked: DatabaseManager.go()
                        Layout.bottomMargin: -50
                    }
                    Item {
                        Layout.preferredWidth: 100
                    }
                    BigButtonFeature {
                        Layout.topMargin: -50
                        implicitWidth: 150
                        implicitHeight: 150
                        icon.width: 120
                        icon.height: 120
                        icon.source: "qrc:/icons/stop2.png"
                        onClicked: DatabaseManager.stop()
                    }
                }

                RowLayout{
                    Layout.leftMargin: 170
                    BigButtonFeature {
                        icon.source: "qrc:/icons/dir-button.png"
                        rotation: 270
                        onClicked: DatabaseManager.left()
                    }
                    BigButtonFeature {
                        icon.source: "qrc:/icons/dir-button.png"
                        rotation: 180
                        onClicked: DatabaseManager.back()
                    }
                    BigButtonFeature {
                        icon.source: "qrc:/icons/dir-button.png"
                        rotation: 90
                        onClicked: DatabaseManager.right()
                    }
                }
            }
        }
    }
}
