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

    Row
    {
        anchors.fill: parent
        anchors.margins: 15
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

            Text {
                text: dialog.textHeader
                font.bold: true
                //font.pixelSize: 16
                wrapMode: Text.WordWrap
            }
            Text {
                // anchors.top: dialogNoHostHeader.bottom
                width: parent.width
                text: dialog.textMain
                //font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
        }
    }

    Button {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        text: "Ok"
        onClicked: { dialog.close(); }
    }
}
