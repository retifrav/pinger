import QtQuick 2.10
import QtQuick.Controls 2.3

Button {
    id: btn

    topPadding: 7
    leftPadding: 15
    rightPadding: 15
    bottomPadding: 7

    contentItem: Text {
        text: btn.text
        color: Styles.buttonsTextColor
    }

    background: Rectangle {
        color: down ? Styles.labelsColorDarker : Styles.labelsColor
        radius: 3
    }
}
