import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtCharts 2.2
import Qt.labs.settings 1.0
//import QtQuick.Controls 1.4 as QQC1
//import QtQuick.Controls.Styles 1.4 as QQC1S
import io.decovar.Backend 1.0
import AppStyle 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1100
    minimumWidth: 900
    height: 650
    minimumHeight: 500
    title: qsTr("pinger")

    Settings {
        id: settings

        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height

        property bool showReport: false
        property bool makeSoundReceived: false
        property bool makeSoundLost: false
    }

    ListModel { id: packetsModel }

    onClosing: {
        backend.closeEvent();
    }

    Backend {
        id: backend

        onGotPingResults: {
            var statusVal = "error";
            var packetColor = Styles.colorError;
            switch (status)
            {
            case 0:
                statusVal = "Received";
                packetColor = Styles.colorReceived;
                break;
            case 1:
                statusVal = "Lost";
                packetColor = Styles.colorLost;
                break;
            }

            packetsModel.append({
                "status": statusVal,
                "time": time,
                "packetColor": packetColor
            });
            //if (packetsModel.count > queueSize) { packetsModel.remove(0); }
            // TODO improve behaviour for resizing window
            packets.positionViewAtEnd();
            //if (packets.contentHeight > packets.height) { packetsModel.remove(0); }

            avgTime.text = averageTime + " ms";

            percentageLost.text = lostPercentage;
            percentageReceived.text = receivedPercentage;
            packetsLost.text = pieLost.value = lostPackets;
            packetsReceived.text = pieReceived.value = receivedPackets;

            if (lostPackets > receivedPackets)
                { glowPacketsPieChart.color = Styles.colorLost; }
            else
                { glowPacketsPieChart.color = Styles.colorReceived; }

            chartSeriesLatencyAxisY.min = minAxisY;
            chartSeriesLatencyAxisY.max = maxAxisY;
//            console.log(minAxisY + " | " + maxAxisY);

            // TODO this value should not be hardcoded
            if (seriesLatency.count > 49)
            {
                chartSeriesLatencyAxisX.min++;
                chartSeriesLatencyAxisX.max++;
                seriesLatency.remove(0);
            }
            //console.log(seriesLatency.at(totalPackets - 1));
            seriesLatency.append(totalPackets, lastPacketTime);

            //console.log(totalPackets);
            //console.log(lastPacketTime);
        }
    }

    Rectangle {
        id: root
        anchors.fill: parent
        color: Styles.mainBackground

        RowLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            LayoutVertical {
                Layout.preferredWidth: parent.width * 0.65

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.1
                    Layout.minimumHeight: 50
                    Layout.maximumHeight: 70

                    RowLayout {
                        anchors.fill: parent
                        //spacing: 15
                        anchors.leftMargin: 20

                        FormLabel {
                            //anchors.verticalCenter: parent.verticalCenter
                            //Layout.alignment: Qt.AlignVCenter
                            text: "Host"
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            //height: btn_ping.height
                            color: "transparent"
                            //border.width: 1

                            TextInput {
                                id: host
                                enabled: btn_ping.visible
                                text: "ya.ru"
                                anchors.verticalCenter: parent.verticalCenter
                                leftPadding: 10
                                rightPadding: 10
                                width: parent.width
                                font.pointSize: 18
                                color: Styles.buttonsTextColor
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
                            Text {
                                text: "PING"
                                font.pixelSize: Styles.secondaryFontSize
                                //font.bold: true
                                color: Styles.buttonsTextColor
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                anchors.fill: parent
                            }
                            onClicked: {
                                if (host.text.length === 0)
                                {
                                    dialogNoHost.show();
                                    return;
                                }
                                else
                                {
                                    clearPreviousData();
                                    btn_ping.visible = false;
                                    backend.on_btn_ping_clicked(host.text);
                                    btn_report.visible = false;
                                    btn_export.visible = false;
                                }
                            }
                        }
                        HalfRoundedButton {
                            id: btn_stop
                            color: Styles.colorLost//buttonDown ? "#CC817E" : "#E57373"
                            //colorGlow: buttonDown ? "#CC817E" : "#E57373"
                            Text {
                                text: "STOP"
                                font.pixelSize: Styles.secondaryFontSize
                                //font.bold: true
                                color: Styles.buttonsTextColor
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                anchors.fill: parent
                            }
                            visible: !btn_ping.visible
                            onClicked: {
                                backend.on_btn_stop_clicked();
                                //loading.visible = false;
                                //loadingAnimation.running = false;
                                btn_ping.visible = true;
                                btn_report.visible = true;
                                btn_export.visible = true;
                                if (settings.showReport === true) { dialogReport.show(); }
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

//                            FormLabel {
//                                anchors.right: parent.right
//                                text: "⋮"
//                            }
                        }

                        ChartView {
                            id: chartSeriesLatency
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            antialiasing: true
                            backgroundColor: "transparent"
                            margins.top: 0
                            margins.bottom: 0
                            margins.left: 0
                            margins.right: 0
                            legend.visible: false

                            Component.onCompleted: {
                                // so it wouldn't start from a point in void
                                scrollRight(20);
                            }

                            ValueAxis {
                                id: chartSeriesLatencyAxisX
                                min: 0
                                max: 50
                                visible: false
                                //minorGridVisible: true
                                //gridVisible: false
                                //lineVisible: false
                                //labelsColor: Styles.labelsColor
                            }

                            ValueAxis {
                                id: chartSeriesLatencyAxisY
                                min: 0
                                max: 100
                                tickCount: 5
                                //visible: false
                                minorGridVisible: true
                                //gridVisible: false
                                //lineVisible: false
                                labelsColor: Styles.labelsColor
                            }

                            SplineSeries {
                                id: seriesLatency
                                axisX: chartSeriesLatencyAxisX
                                axisY: chartSeriesLatencyAxisY
                                width: 4
                                color: Styles.colorReceived
                            }
                        }
                        // labels are glowing too, unfortunatelly
//                        Glow {
//                            anchors.fill: chartSeriesLatency
//                            radius: 18
//                            samples: 37
//                            color: Styles.labelsColor
//                            source: chartSeriesLatency
//                        }
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
                                id: lbl_headerPackets
                                anchors.left: parent.left
                                text: "Packets"
                            }

                            IconButton {
                                id: btn_pieChart
                                anchors.right: parent.right//lbl_headerPackets.left
                                Layout.preferredWidth: lbl_headerPackets.height
                                Layout.preferredHeight: lbl_headerPackets.height
                                source: packets.visible
                                        ? "qrc:/images/pie.png"
                                          : "qrc:/images/table.png"

                                onClicked: {
                                    packets.visible = !packets.visible;
                                }
                            }
//                            Image {
//                                id: loading
//                                anchors.right: lbl_headerPackets.left
//                                Layout.preferredWidth: lbl_headerPackets.height
//                                Layout.preferredHeight: lbl_headerPackets.height
//                                source: "qrc:/images/loading.png"
//                                fillMode: Image.PreserveAspectFit
//                                visible: false
//                                RotationAnimator {
//                                    id: loadingAnimation
//                                    target: loading;
//                                    from: 0;
//                                    to: 360;
//                                    duration: 2000
//                                    loops: Animation.Infinite
//                                }
//                            }
//                            FormLabel {
//                                id: lbl_headerPackets
//                                anchors.right: parent.right
//                                text: "⋮"
//                            }
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
                            Layout.bottomMargin: -20
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            //interactive: false

                            //flickableDirection: Flickable.VerticalFlick
                            //boundsBehavior: Flickable.StopAtBounds
                            //ScrollBar.vertical: ScrollBar {}
                            clip: true

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
                            Layout.fillHeight: true
                            Layout.topMargin: -10
                            Layout.bottomMargin: -20
                            color: "transparent"
                            visible: !packets.visible

                            ChartView {
                                id: packetsPieChart
                                legend.visible: false
                                antialiasing: true
                                backgroundColor: "transparent"

                                // crutch for ChartView margins
                                x: -10; width: parent.width + 20;
                                y: -10; height: parent.height + 20;

                                margins.top: 0
                                margins.bottom: 0
                                margins.right: 0
                                margins.left: 0

                                PieSeries {
                                    id: packetsPieChartSeries
                                    size: 1.0
                                    holeSize: 0.7

                                    PieSlice {
                                        id: pieLost
                                        label: "Lost"
                                        value: 0
                                        color: Styles.colorLost
                                        borderColor: color
                                    }

                                    PieSlice {
                                        id: pieReceived
                                        label: "Received"
                                        value: 1
                                        color: Styles.colorReceived
                                        borderColor: color
                                    }
                                }
                            }
                            Glow {
                                id: glowPacketsPieChart
                                anchors.fill: packetsPieChart
                                radius: 18
                                samples: 37
                                color: Styles.colorReceived
                                source: packetsPieChart
                                visible: pieLost.value === 0 || pieReceived === 0
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
                                color: Styles.colorLost
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
                                color: Styles.colorLost
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
                                color: Styles.colorReceived
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
                                color: Styles.colorReceived
                                visible: !percentageReceived.visible
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        switchToPacketsReceived(false);
                                    }
                                }
                            }
                        }
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
                            onClicked: { dialogSettings.show(); }
                        }
//                        IconButton {
//                            id: btn_reload
//                            source: "qrc:/images/reload.png"
//                            //onClicked: { settings.makeSoundReceived = true; }
//                        }
                        IconButton {
                            id: btn_report
                            source: "qrc:/images/info.png"
                            onClicked: { dialogReport.show(); }
                            visible: false
                        }
                        IconButton {
                            id: btn_export
                            source: "qrc:/images/export.png"
                            visible: false
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

    MessageBox {
        id: dialogNoHost
        title: "No host provided"
        textHeader: "You haven't provided any host to ping."
        textMain: "In order to ping some host, you should provide either its domain name or its IP address."
        source: "/images/warning.png"
    }

    Window {
        id: dialogSettings
        visible: false
        modality: Qt.WindowModal

        width: 500
        minimumWidth: width
        maximumWidth: width
        height: 300
        minimumHeight: height
        maximumHeight: height

        Rectangle {
            anchors.fill: parent
            color: Styles.regionBackground
            border.color: Styles.mainBackground
            border.width: 2

            ColumnLayout
            {
                anchors.fill: parent
                anchors.topMargin: 15
                anchors.leftMargin: 20
                anchors.rightMargin: 15
                anchors.bottomMargin: 15

                DialogText {
                    text: "Settings"
                    font.pixelSize: 20
                    font.bold: true
                }

                ColumnLayout {
                    DialogText {
                        text: "General"
                        font.bold: true
                    }
                    Item { height: 3 }
                    DialogSwitch {
                        id: switchShowReport
                        text: qsTr("show report automatically")
                        checked: settings.showReport
                    }

                }
                ColumnLayout {
                    DialogText {
                        text: "Sounds"
                        font.bold: true
                    }
                    Item { height: 3 }
                    DialogSwitch {
                        id: switchSoundReceived
                        text: qsTr("packet received")
                        checked: settings.makeSoundReceived
                    }
                    DialogSwitch {
                        id: switchSoundLost
                        text: qsTr("packet lost")
                        checked: settings.makeSoundLost
                    }
                }

                Row {
                    Layout.fillWidth: true
                    anchors.bottom: parent.bottom
                    layoutDirection: Qt.RightToLeft
                    spacing: 5

                    DialogButton {
                        text: "Save"
                        onClicked: {
                            settings.showReport = switchShowReport.checked;
                            settings.makeSoundReceived = switchSoundReceived.checked;
                            settings.makeSoundLost = switchSoundLost.checked;
                            dialogSettings.close();
                        }
                    }
                    DialogButton {
                        text: "Cancel"
                        onClicked: {
                            dialogSettings.close();
                            switchShowReport.checked = settings.showReport;
                            switchSoundReceived.checked = settings.makeSoundReceived;
                            switchSoundLost.checked = settings.makeSoundLost;
                        }
                    }
                }
            }
        }
    }

    Window {
        id: dialogReport
        visible: false
        modality: Qt.WindowModal

        width: 600
        minimumWidth: width
        maximumWidth: width
        height: 300
        minimumHeight: height
        maximumHeight: height

        Rectangle {
            anchors.fill: parent
            color: Styles.regionBackground
            border.color: Styles.mainBackground
            border.width: 2

            ColumnLayout
            {
                anchors.fill: parent
                anchors.topMargin: -15
                anchors.leftMargin: 20
                anchors.rightMargin: 15
                anchors.bottomMargin: 15

                DialogText {
                    text: "Report"
                    font.pixelSize: 20
                    font.bold: true
                }

                Row {
                    Layout.fillWidth: true
                    anchors.bottom: parent.bottom
                    layoutDirection: Qt.RightToLeft
                    spacing: 5

                    DialogButton {
                        text: "Close"
                        onClicked: {
                            dialogReport.close();
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        // start pinging right after launching
        if (host.text.length !== 0) { btn_ping.clicked(); }
    }

    function clearPreviousData()
    {
        packetsModel.clear();

        avgTime.text = "0 ms";
        packetsLost.text = "0";
        packetsReceived.text = "0";
        percentageLost.text = "0%";
        percentageReceived.text = "0%";
        pieLost.value = 0;
        pieReceived.value = 0;

        seriesLatency.removePoints(0, seriesLatency.count);
        //seriesLatency.append(0, 0);

        //loadingAnimation.running = true;
        //loading.visible = true;
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
