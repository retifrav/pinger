import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root

    topPadding: 7
    leftPadding: 15
    rightPadding: 15
    bottomPadding: 7

    contentItem: Text {
        text: root.text
        font.pointSize: Styles.dialogFontSize
        color: Styles.buttonsTextColor
    }

    background: Rectangle {
        color: root.down ? Styles.labelsColorDarker : Styles.labelsColor
        radius: 3
    }
}
