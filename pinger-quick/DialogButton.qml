import QtQuick//2.15
import QtQuick.Controls//2.15

Button {
    id: root

    topPadding: 7
    leftPadding: 15
    rightPadding: 15
    bottomPadding: 7

    contentItem: Text {
        id: buttonText
        text: root.text
        font.pointSize: Styles.dialogFontSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: Styles.buttonsTextColor
    }

    background: Rectangle {
        implicitWidth: buttonText.contentWidth + 20
        implicitHeight: buttonText.contentHeight * 1.6//Styles.dialogFontSize * 2
        color: root.down ? Styles.labelsColorDarker : Styles.labelsColor
        radius: 3
    }
}
