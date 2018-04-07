import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

ToolButton {
    id: button
    text: ""
    //implicitWidth: height
    //Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
    property string source

    background: Rectangle {
        id: btnBack
        implicitWidth: 40
        implicitHeight: 40
        color: down ? "#45535D" : "#4E5D6A"
        radius: 8
        opacity: down ? 1 : 0
    }

    onHoveredChanged: {
        hovered ? btnBack.opacity = 0.5 : btnBack.opacity = 0;
    }

    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        sourceSize.height: button.background.height - 6
        height: sourceSize.height
        source: button.source
    }
//    ColorOverlay {
//        anchors.fill: image
//        source: image
//        color: Styles.labelsColor
//    }
}
