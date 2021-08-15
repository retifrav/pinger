import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

ToolButton {
    id: root

    text: ""
    //implicitWidth: height
    //Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
    property string source
    property string tooltipText

    background: Rectangle {
        id: btnBack
        implicitWidth: 40
        implicitHeight: 40
        color: root.down ? "#45535D" : "#4E5D6A"
        radius: 8
        opacity: root.down ? 1 : 0
    }

    onHoveredChanged: {
        hovered ? btnBack.opacity = 0.5 : btnBack.opacity = 0;
    }

    ToolTip {
        delay: Styles.toolTipDelay
        timeout: Styles.toolTipTimeout
        visible: root.hovered
        font.pointSize: Styles.consoleFontSize
        text: root.tooltipText
    }

    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: root
        sourceSize.height: root.background.height - 6
        height: sourceSize.height
        source: root.source
    }
//    ColorOverlay {
//        anchors.fill: image
//        source: image
//        color: Styles.labelsColor
//    }
}
