import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.3

Window {
    id: dialog

    property string title
    property string source
    property string textHeader
    property string textMain

    title: dialog.title
    modality: Qt.WindowModal

    width: 400
    minimumWidth: width
    maximumWidth: width
    height: 160
    minimumHeight: height
    maximumHeight: height

    Rectangle {
        anchors.fill: parent
        color: Styles.regionBackground
        border.color: Styles.mainBackground
        border.width: 3

        Item {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.topMargin: 20
            anchors.rightMargin: 15
            anchors.bottomMargin: 15

            Row
            {
                anchors.fill: parent
                spacing: 15

                Image {
                    width: parent.width / 5
                    height: width
                    fillMode: Image.PreserveAspectFit
                    source: dialog.source
                }

                Column
                {
                    spacing: 5

                    DialogText {
                        text: dialog.textHeader
                        font.bold: true
                    }
                    DialogText {
                        width: parent.width
                        text: dialog.textMain
                    }
                }
            }

            DialogButton {
                id: btn
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                text: "Ok"
                onClicked: { dialog.close(); }
            }
        }
    }
}
