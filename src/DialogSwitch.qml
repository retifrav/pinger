import QtQuick
import QtQuick.Controls

Switch {
    id: root

    property string helpTooltip

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

    contentItem: Row {
        leftPadding: root.indicator.width + root.spacing
        DialogText {
            text: root.text
            font.pointSize: Styles.dialogFontSize
            //verticalAlignment: Text.AlignVCenter
        }
        FormTextWithHover {
            leftPadding: 3
            color: Styles.buttonsTextColor
            //verticalAlignment: Text.AlignVCenter
            text: "(?)"
            font.pointSize: Styles.dialogFontSize
            tooltipText: qsTr(helpTooltip)
            visible: helpTooltip.length > 0
        }
    }
}
