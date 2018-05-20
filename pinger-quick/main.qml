import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
//import QtQuick.Controls 1.4 as QQC1
//import QtQuick.Controls.Styles 1.4 as QQC1S
import io.qt.Backend 1.0
import AppStyle 1.0

ApplicationWindow {
    visible: true
    width: 1100
    minimumWidth: 900
    height: 650
    minimumHeight: 500
    title: qsTr("pinger")

    ListModel { id: packetsModel }

    onClosing: {
        backend.closeEvent();
    }

    Backend {
        id: backend

        onGotPingResults: {
            packetsModel.append({
                "status":status,
                "time":time,
                "packetColor":packetColor
            });
            //if (packetsModel.count > queueSize) { packetsModel.remove(0); }
            // TODO improve behaviour for resizing window
            if (packets.contentHeight > packets.height) { packetsModel.remove(0); }

            avgTime.text = averageTime;
            percentageLost.text = lostPercentage;
            percentageReceived.text = receivedPercentage;
            packetsLost.text = lostPackets;
            packetsReceived.text = receivedPackets;
        }
    }

    Rectangle {
        id: root
        anchors.fill: parent
        color: "#263238"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            LayoutVertical {
                Layout.preferredWidth: parent.width * 0.65

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.15
                    Layout.maximumHeight: 80

                    RowLayout {
                        anchors.fill: parent
                        //spacing: 15
                        anchors.leftMargin: 20

                        FormLabel {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Host"
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            //height: btn_ping.height
                            color: "transparent"
                            //border.width: 1

                            TextInput {
                                id: host
                                text: "ya.ru"
                                anchors.verticalCenter: parent.verticalCenter
                                leftPadding: 10
                                rightPadding: 10
                                width: parent.width
                                font.pointSize: 18
                                color: "#B3BFC5"
                                clip: true
                            }
                        }

//                        Rectangle {
//                            Layout.fillWidth: true
//                            height: btn_ping.height
//                            color: "transparent"
//                            //border.width: 1

//                            Rectangle {
//                                width: parent.width
//                                height: 2
//                                anchors.bottom: parent.bottom
//                                color: "#667D89"
//                            }

//                            TextInput {
//                                id: host
//                                anchors.verticalCenter: parent.verticalCenter
//                                leftPadding: 10
//                                rightPadding: 10
//                                width: parent.width
//                                font.pixelSize: 18
//                                color: "#B3BFC5"
//                                clip: true
//                            }
//                        }

                        HalfRoundedButton {
                            id: btn_ping
                            Layout.preferredWidth: parent.width * 0.3
                            Layout.preferredHeight: parent.height// * 0.55
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                text: "PING"
                                font.pixelSize: Styles.secondaryFontSize
                                //font.bold: true
                                color: "#CFD8DC"
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                anchors.fill: parent
                            }
                            onClicked: {
                                packetsModel.clear();

                                avgTime.text = "0 ms";
                                percentageLost.text = "0%";
                                percentageReceived.text = "0%";

                                visible = false;
                                loadingAnimation.running = true;
                                loading.visible = true;
                                backend.on_btn_ping_clicked(host.text);
                            }
                        }
                        HalfRoundedButton {
                            id: btn_stop
                            Layout.preferredWidth: parent.width * 0.3
                            Layout.preferredHeight: parent.height// * 0.55
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                text: "STOP"
                                font.pixelSize: Styles.secondaryFontSize
                                //font.bold: true
                                color: "#CFD8DC"
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                anchors.fill: parent
                            }
                            visible: !btn_ping.visible
                            onClicked: {
                                backend.on_btn_stop_clicked();
                                loading.visible = false;
                                loadingAnimation.running = false;
                                btn_ping.visible = true;
                            }
                        }
                    }
                }

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    LayoutVertical {
                        anchors.fill: parent
                        anchors.margins: 20

                        LayoutHorizontal {
                            anchors.fill: parent

                            FormLabel {
                                anchors.left: parent.left
                                text: "Time, ms / Packets"
                            }

                            FormLabel {
                                anchors.right: parent.right
                                text: "⋮"
                            }
                        }
                    }
                }

//                    LayoutRegion {
//                        Layout.preferredWidth: parent.width * 0.3
//                        Layout.fillHeight: true

//                        FormLabel {
//                            topPadding: 20
//                            leftPadding: 20
//                            text: "Percentage"
//                        }

//                        FormLabel {
//                            anchors.right: parent.right
//                            topPadding: 15
//                            rightPadding: 10
//                            text: "⋮"
//                            font.bold: true
//                        }
//                    }
                }

            LayoutVertical {
                Layout.preferredWidth: parent.width * 0.4
                Layout.maximumWidth: 450

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    LayoutVertical {
                        anchors.fill: parent
                        anchors.margins: 20

                        LayoutHorizontal {
                            anchors.fill: parent

                            FormLabel {
                                anchors.left: parent.left
                                text: "Packets"
                            }

                            Image {
                                id: loading
                                anchors.right: lbl_headerPackets.left
                                Layout.preferredWidth: lbl_headerPackets.height
                                Layout.preferredHeight: lbl_headerPackets.height
                                source: "qrc:/images/loading.png"
                                fillMode: Image.PreserveAspectFit
                                visible: false
                                RotationAnimator {
                                    id: loadingAnimation
                                    target: loading;
                                    from: 0;
                                    to: 360;
                                    duration: 2000
                                    loops: Animation.Infinite
                                }
                            }
                            FormLabel {
                                id: lbl_headerPackets
                                anchors.right: parent.right
                                text: "⋮"
                            }
                        }

                        Item {
                            id: statsHeader
                            Layout.fillWidth: true
                            Layout.topMargin: 15

                            FormText {
                                anchors.left: parent.left
                                text: "Status";
                            }
                            FormText {
                                anchors.right: parent.right
                                text: "Time";
                            }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            height: 3
                            color: Styles.labelsColor
                        }

                        ListView {
                            id: packets
                            model: packetsModel
                            Layout.topMargin: -10
                            Layout.bottomMargin: 10
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            interactive: false

                            delegate: Item {
                                height: 30
                                width: parent.width
                                FormText {
                                    anchors.left: parent.left
                                    text: status
                                }
                                Rectangle {
                                    id: packetStatus
                                    anchors.centerIn: parent
                                    width: 15
                                    height: 15
                                    radius: width * 0.5
                                    color: packetColor
                                }
                                Glow {
                                    anchors.fill: packetStatus
                                    radius: 20
                                    samples: 20
                                    color: packetStatus.color
                                    spread: 0.1
                                    source: packetStatus
                                }
                                FormText {
                                    anchors.right: parent.right
                                    text: time
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            anchors.bottom: statsNumbers.top
                            anchors.bottomMargin: 10
                            height: 3
                            color: Styles.labelsColor
                        }
                        Row {
                            id: statsNumbers
                            Layout.fillWidth: true
                            anchors.bottom: parent.bottom
                            layoutDirection: Qt.RightToLeft
                            spacing: 5

                            FormText {
                                id: avgTime;
                                text: "0 ms";
                            }
                            FormText { text: "|"; }
                            FormText {
                                id: percentageLost
                                text: "0%";
                                color: "#E57373"
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        switchToPacketsLost(true);
                                    }
                                }
                            }
                            FormText {
                                id: packetsLost
                                text: "0";
                                color: "#E57373"
                                visible: !percentageLost.visible
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        switchToPacketsLost(false);
                                    }
                                }
                            }
                            FormText { text: "|"; }
                            FormText {
                                id: percentageReceived
                                text: "0%";
                                color: "#81C784"
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        switchToPacketsReceived(true);
                                    }
                                }
                            }
                            FormText {
                                id: packetsReceived
                                text: "0";
                                color: "#81C784"
                                visible: !percentageReceived.visible
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        switchToPacketsReceived(false);
                                    }
                                }
                            }
                        }

// --- option with table view
//                        QQC1.TableView {
//                            id:tabl
//                            Layout.fillWidth: true
//                            Layout.fillHeight: true
//                            backgroundVisible: false
//                            frameVisible: false
////                            headerVisible: false

//                            style: QQC1S.TableViewStyle {
//                                backgroundColor: "transparent"
//                                alternateBackgroundColor: "transparent"
//                                textColor: Styles.labelsColor
////                                frame: Rectangle {
////                                    color: "transparent"
////                                }
//                                headerDelegate: Rectangle {
//                                    height: 30
////                                    width: textItem.implicitWidth
//                                    color: "transparent"
//                                    Text {
//                                        id: textItem
//                                        anchors.fill: parent
//                                        verticalAlignment: Text.AlignVCenter
//                                        horizontalAlignment: styleData.textAlignment
////                                        anchors.leftMargin: 12
//                                        text: styleData.value
//                                        font.pointSize: Styles.secondaryFontSize
//                                        elide: Text.ElideRight
//                                        color: textColor
////                                        renderType: Text.NativeRendering
//                                    }
//                                    Rectangle {
//                                        anchors.right: parent.right
//                                        anchors.top: parent.top
//                                        anchors.bottom: parent.bottom
////                                        anchors.bottomMargin: 1
////                                        anchors.topMargin: 1
////                                        width: 1
////                                        color: "#ccc"
//                                    }
//                                }
//                            }

//                            QQC1.TableViewColumn {
//                                role: "status"
//                                title: "Status"
//                                width: tabl.width / 3
//                            }
//                            QQC1.TableViewColumn {
//                                role: "statusImg"
//                                //title: "Image"
//                                width: tabl.width / 3
//                            }
//                            QQC1.TableViewColumn {
//                                role: "time"
//                                title: "Time"
//                                width: tabl.width / 3
//                            }
//                            model: packetsModel
//                            rowDelegate: Rectangle {
//                                height: 30
//                                color: "transparent"
//                            }
//                            itemDelegate: Item {
//                                Text {
////                                    anchors.verticalCenter: parent.verticalCenter
//                                    color: styleData.textColor
//                                    elide: styleData.elideMode
//                                    text: styleData.value
//                                    font.pixelSize: Style.secondaryFontSize
//                                    //topPadding: 15
//                                    //bottomPadding: 15
//                                }
//                            }
//                        }
// ---
                    }
                }

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.15
                    Layout.maximumHeight: 80

                    LayoutHorizontal {
                        anchors.centerIn: parent
                        spacing: parent.width * 0.05 + parent.width / btn_settings.width

                        IconButton {
                            id: btn_settings
                            source: "qrc:/images/settings.png"
                        }
                        IconButton {
                            id: btn_reload
                            source: "qrc:/images/reload.png"
                        }
                        IconButton {
                            id: btn_info
                            source: "qrc:/images/info.png"
                        }
                        IconButton {
                            id: btn_export
                            source: "qrc:/images/export.png"
                        }
                    }
                }
            }

//            LayoutRegion {
//                Layout.row: 1
//                Layout.column: 0
//                Layout.rowSpan: 2
//                Layout.fillWidth: true
//                Layout.fillHeight: true
//            }

//            LayoutRegion {
//                Layout.row: 1
//                Layout.column: 1
//                Layout.rowSpan: 2
//                Layout.fillWidth: true
//                Layout.fillHeight: true
//            }

//            LayoutRegion {
//                Layout.row: 2
//                Layout.column: 2
//                //Layout.fillWidth: true
//                Layout.preferredWidth: parent.width * 0.3
//                Layout.fillHeight: true
//            }
        }
    }


    function switchToPacketsLost(toPackets)
    {
        if (toPackets === true)
        { percentageLost.visible = false; }
        else
        { percentageLost.visible = true; }
    }

    function switchToPacketsReceived(toPackets)
    {
        if (toPackets === true)
        { percentageReceived.visible = false; }
        else
        { percentageReceived.visible = true; }
    }
}
