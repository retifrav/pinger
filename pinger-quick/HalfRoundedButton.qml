import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Button {
    id: control

    leftPadding: padding + 10
    topPadding: padding + 10
    rightPadding: padding + 10
    bottomPadding: padding + 10

    property alias color: back.color

    contentItem: Text {
        text: control.text
        font.pixelSize: 14
    }

    background: Rectangle {
        color: "transparent"
        HalfRounded {
            id: back
            color: down ? "#677A86" : Styles.labelsColor
        }
        Glow {
            id: glow
            anchors.fill: back
            radius: 15
            samples: 15
            color: control.down ? "#677A86" : Styles.labelsColor
            spread: 0.1
            source: back
            visible: false
        }
    }

    onHoveredChanged: {
//        hovered ? back.color = "#677A86" : back.color = Styles.labelsColor;
        hovered ? glow.visible = true : glow.visible = false;
    }
}
