import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root

    property alias windowTitle: root.title
    property alias windowWidth: root.width
    property alias windowHeight: root.height
    default property alias contents: placeholder.data

    modality: Qt.WindowModal

    width: Styles.dialogWindowWidth
    minimumWidth: width
    maximumWidth: width
    height: Styles.dialogWindowHeight
    minimumHeight: height
    maximumHeight: height

    Rectangle {
        anchors.fill: parent
        color: Styles.regionBackground
        border.color: Styles.mainBackground
        border.width: Styles.dialogBorderWidth

        Item {
            id: placeholder
            anchors.fill: parent
        }
    }

    Shortcut {
        sequences: [ "Esc", "Space" ]
        onActivated: root.close()
    }
}
