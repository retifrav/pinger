import QtQuick 2.10
import QtQuick.Controls 2.3

Switch {
    id: control

    indicator: Rectangle {
        id: indicatorBase
        implicitWidth: 55
        implicitHeight: 16
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: height / 2
        color: control.checked ? Styles.colorReceived : Styles.colorLost
        border.color: control.checked ? Styles.colorReceived : Styles.colorLost

        Rectangle {
            x: control.checked ? parent.width - width : 0
            anchors.verticalCenter: indicatorBase.verticalCenter
            width: indicatorBase.height * 1.8
            height: width
            radius: width / 2
            color: control.down ? "#cccccc" : "#ffffff"
            border.color: "#cccccc"//control.checked ? Styles.colorReceived : Styles.colorLost
        }
    }

    contentItem: DialogText {
        text: control.text
        font: control.font
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
