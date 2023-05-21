import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt5Compat.GraphicalEffects

ToolButton {
    id: root

    text: ""
    //implicitWidth: height
    //Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
    property alias imageSource: img.source
    property alias tooltipText: tltp.text

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
        id: tltp
        delay: Styles.toolTipDelay
        timeout: Styles.toolTipTimeout
        visible: root.hovered
        font.pointSize: Styles.consoleFontSize
    }

    Image {
        id: img
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: root
        sourceSize.height: root.background.height - 6
        height: sourceSize.height
        source: root.source
    }
//    ColorOverlay {
//        anchors.fill: img
//        source: img
//        color: Styles.labelsColor
//    }
}
