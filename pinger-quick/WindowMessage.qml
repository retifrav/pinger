import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: root

    property string title
    property string statusImage
    property string textHeader
    property string textMain

    title: root.title
    modality: Qt.WindowModal

    width: 400
    minimumWidth: width
    maximumWidth: width
    height: 180
    minimumHeight: height
    maximumHeight: height

    Rectangle {
        anchors.fill: parent
        color: Styles.regionBackground
        border.color: Styles.mainBackground
        border.width: Styles.dialogBorderWidth

        Item {
            anchors.fill: parent
            anchors.leftMargin: Styles.dialogPaddingLeft
            anchors.topMargin: Styles.dialogPaddingTop
            anchors.rightMargin: Styles.dialogPaddingRight
            anchors.bottomMargin: Styles.dialogPaddingBottom

            Row
            {
                anchors.fill: parent
                spacing: 15

                Image {
                    id: img
                    width: parent.width / 8
                    height: width
                    fillMode: Image.PreserveAspectFit
                    source: root.statusImage
                }

                Column
                {
                    width: parent.width - img.width - parent.spacing * 1.5
                    spacing: 10

                    DialogText {
                        width: parent.width
                        text: root.textHeader
                        font.pointSize: Styles.secondaryFontSize
                        font.bold: true
                    }
                    DialogText {
                        width: parent.width
                        text: root.textMain
                    }
                }
            }

            DialogButton {
                id: btn
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                text: "OK"
                onClicked: { root.close(); }
            }
        }
    }
}
