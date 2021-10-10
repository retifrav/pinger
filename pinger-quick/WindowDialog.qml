import QtQuick//2.15
import QtQuick.Window//2.15
import QtQuick.Controls//2.15

Window {
    id: root

    property string title
    default property alias contents: placeholder.data

    title: root.title
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
        sequence: "Esc"
        onActivated: root.close()
    }
}
