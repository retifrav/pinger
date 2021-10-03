import QtQuick//2.15
import QtQuick.Controls//2.15

Item {
    anchors.fill: parent

    property alias color: backTemplate.color

    Rectangle {
        id: backTemplate
        anchors.fill: parent
        radius: 8
    }
    Rectangle {
        color: backTemplate.color
        width: backTemplate.radius
        anchors.top: backTemplate.top
        anchors.left: backTemplate.left
        anchors.bottom: backTemplate.bottom
    }
}
