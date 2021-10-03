import QtQuick//2.15
import QtQuick.Controls//2.15

Switch {
    id: root

    indicator: Rectangle {
        id: indicatorBase
        implicitWidth: 55
        implicitHeight: 16
        x: root.leftPadding
        y: parent.height / 2 - height / 2
        radius: height / 2
        color: root.checked ? Styles.colorReceived : Styles.colorLost
        border.color: root.checked ? Styles.colorReceived : Styles.colorLost

        Rectangle {
            x: root.checked ? parent.width - width : 0
            anchors.verticalCenter: indicatorBase.verticalCenter
            width: indicatorBase.height * 1.8
            height: width
            radius: width / 2
            color: root.down ? "#cccccc" : "#ffffff"
            border.color: "#cccccc"//root.checked ? Styles.colorReceived : Styles.colorLost
        }
    }

    contentItem: DialogText {
        text: root.text
        font.pointSize: Styles.dialogFontSize
        verticalAlignment: Text.AlignVCenter
        leftPadding: root.indicator.width + root.spacing
    }
}
