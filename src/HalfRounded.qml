import QtQuick
import QtQuick.Controls

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
