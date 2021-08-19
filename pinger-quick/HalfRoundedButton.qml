import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15

Button {
    id: root

    leftPadding: padding + 10
    topPadding: padding + 10
    rightPadding: padding + 10
    bottomPadding: padding + 10

    property alias color: back.color
    property alias colorGlow: glow.color
    property alias buttonDown: root.down

    contentItem: Text {
        text: root.text
        font.pointSize: Styles.headerFontSize
        font.bold: true
        color: Styles.buttonsTextColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }

    background: Rectangle {
        color: "transparent"
        HalfRounded {
            id: back
            color: Styles.labelsColor//root.down ? "#677A86" : Styles.labelsColor
        }
        Glow {
            id: glow
            anchors.fill: back
            radius: 15
            samples: 15
            color: back.color//root.down ? "#677A86" : Styles.labelsColor
            spread: 0.1
            source: back
            visible: false
        }
    }

    onHoveredChanged: {
//        root.hovered ? back.color = "#677A86" : back.color = Styles.labelsColor;
        root.hovered ? glow.visible = true : glow.visible = false;
    }
}
