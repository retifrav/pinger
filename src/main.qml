import QtCore
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtCharts
import Qt.labs.platform
import dev.decovar.Backend 1.0
import dev.decovar.Clipboard 1.0
import ApplicationStyles 1.0

ApplicationWindow {
    id: mainWindow
    // QTBUG-35244: both `visible` and `visibility` need to be set
    visible: true
    visibility: Window.Maximized
    width: 1100
    minimumWidth: 900
    height: 650
    minimumHeight: 500
    /*title: mainWindow.licensedTo.length === 0
        ? `${applicationName} [unregistered]`
        : applicationName*/
    title: debugMode ? `${applicationName} [debug mode]` : applicationName

    readonly property string applicationName: backend.getApplicationName()
    readonly property var applicationVersion: backend.getVersionInfo()
    readonly property string applicationVersionString: `<b>Version:</b> ${applicationVersion.major}.${applicationVersion.minor}.${applicationVersion.revision}`
        .concat(`<br><b>Commit:</b> ${applicationVersion.commit}`)
        .concat(`<br><b>Built on:</b> ${applicationVersion.date}`)
    readonly property bool enableMenuBar: true//Qt.platform.os === "osx"
    //readonly property string licensedTo: backend.getLicensedTo()

    property int latencyChartWidth: backend.getQueueSize() // chart view width in packets
    property int minimumRequiredPackets: 50
    property int latencyFactor: settings.usingPingUtility === true ? 1 : 4;
    property int latencyFactorAvg: latencyFactor === 1 ? 1 : latencyFactor / 2;

    property var results

    // we don't want everyone to spam the same host, so every new user (without settings saved yet)
    // gets a random host from this list, which is then saved to settings
    property var hostnames: [
        "ya.ru",
        "vk.com",
        "mail.ru",
        "google.com",
        "youtube.com",
        "microsoft.com",
        "live.com",
        "bing.com",
        "amazon.com",
        "aws.com",
        "apple.com",
        "discord.com",
        "ftp.se.debian.org",
        "ftp.de.debian.org",
        "ftp.us.debian.org",
        "pornhub.com"
    ]

    Settings {
        id: settings

        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height

        property string hostname

        property bool usingPingUtility: true

        property bool showReport: false
        property bool makeSoundReceived: false
        property bool makeSoundLost: false

        property bool showReceivedAsPercentage: false
        property bool showLostAsPercentage: true

        property int visibility: Window.Maximized
    }

    // application menu, not native
    /*menuBar: MenuBar {
        Menu {
           title: qsTr("&File")
           Action { text: qsTr("&Save") }
           Action { text: qsTr("Save &as...") }
           MenuSeparator { }
           Action { text: qsTr("&Quit") }
       }
       Menu {
           title: qsTr("&Help")
           Action { text: qsTr("&About") }
       }
    }*/

    // native application menu, mostly for Mac OS
    MenuBar {
        id: menuBar

        Menu {
            id: fileMenu
            title: qsTr("File")
            visible: mainWindow.enableMenuBar

            MenuItem {
                text: qsTr("Preferences...")
                role: MenuItem.PreferencesRole
                shortcut: "Ctrl+,"
                onTriggered: showSettingsDialog()
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Quit")
                role: MenuItem.QuitRole
                onTriggered: Qt.quit()
            }
        }

        Menu {
            id: editMenu
            title: qsTr("?")
            visible: mainWindow.enableMenuBar

            MenuItem {
                text: qsTr("About")
                role: MenuItem.AboutRole
                onTriggered: { dialogAbout.show(); }
            }

            MenuItem {
                text: qsTr("About Qt")
                role: MenuItem.AboutQtRole
                onTriggered: {
                    //console.debug(Qt.platform.os, Qt.platform.pluginName)
                    backend.showAboutQt();
                }
                //visible: debugMode
            }

            MenuItem {
                text: "Source code"
                onTriggered: {
                    Qt.openUrlExternally(applicationVersion.repository)
                }
            }

            MenuItem {
                text: qsTr("License (GPLv3)")
                onTriggered: {
                    //dialogLicense.show();
                    Qt.openUrlExternally(applicationVersion.license)
                }
            }
        }
    }

    ListModel { id: packetsModel }

    Backend {
        id: backend

        onGotPingResults: (
            status,
            time,
            averageTime,
            lostPercentage,
            receivedPercentage,
            lostPackets,
            receivedPackets,
            totalPackets,
            lastPacketTime,
            minAxisY,
            maxAxisY
        ) => {
            let statusVal = "Error";
            let packetColor = Styles.colorError;
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
            //if (packetsModel.count > backend.getQueueSize()) { packetsModel.remove(0); }
            // TODO improve behaviour for resizing window
            if (packets.atYEnd) { packets.positionViewAtEnd(); }
            //if (packets.contentHeight > packets.height) { packetsModel.remove(0); }

            avgTime.text = `~${averageTime} ms`;

            percentageLost.text = lostPercentage;
            percentageReceived.text = receivedPercentage;
            packetsLost.text = pieLost.value = lostPackets;
            packetsReceived.text = pieReceived.value = receivedPackets;

            if (lostPackets > receivedPackets)
            {
                glowPacketsPieChart.color = Styles.colorLost;
            }
            else
            {
                glowPacketsPieChart.color = Styles.colorReceived;
            }

            //console.log(minAxisY + " | " + maxAxisY);
            chartSeriesLatencyAxisY.min = minAxisY;
            chartSeriesLatencyAxisY.max = maxAxisY;

            if (seriesLatency.count > latencyChartWidth)
            {
                //chartSeriesLatencyAxisX.min++;
                //chartSeriesLatencyAxisX.max++;

                for (let i = 0; i < seriesLatency.count - 1; i++)
                {
                    seriesLatency.replace(
                        seriesLatency.at(i).x,
                        seriesLatency.at(i).y,
                        i,
                        seriesLatency.at(i+1).y
                    );
                }
                seriesLatency.replace(
                    seriesLatency.at(seriesLatency.count - 1).x,
                    seriesLatency.at(seriesLatency.count - 1).y,
                    seriesLatency.count - 1,
                    lastPacketTime
                );
            }
            else
            {
                seriesLatency.append(totalPackets - 1, lastPacketTime);
            }

            //console.log(totalPackets);
            //console.log(lastPacketTime);
        }

        onGotError: (errorMessage) =>
        {
            addToLog(errorMessage.trim());

            packetsModel.append({
                "status": "Error",
                "time": "-",
                "packetColor": Styles.colorError
            });
            // TODO improve behaviour for resizing window
            if (packets.atYEnd) { packets.positionViewAtEnd(); }
        }
    }

    Clipboard {
        id: clipboard

        onDataChanged: {
            if (debugMode === true)
            {
                console.debug("Clipboard data changed");
            }
        }

        onSelectionChanged: {
            if (debugMode === true)
            {
                console.debug("Clipboard selection changed");
            }
        }
    }

    Drawer {
        id: drawer
        edge: Qt.BottomEdge
        width: root.width
        height: root.height / 3

        Rectangle {
            anchors.fill: parent
            color: Styles.regionBackground
            border.color: "transparent"//Styles.regionBackground

            ScrollView {
                anchors.fill: parent
                anchors.topMargin: 4
                anchors.bottomMargin: 4
                anchors.leftMargin: 0
                anchors.rightMargin: 0

                TextArea {
                    id: applicationLog
                    readOnly: true
                    color: "#AAA"//Styles.buttonsTextColor
                    font.pointSize: Styles.consoleFontSize
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    selectByMouse: true

                    background: Rectangle {
                        color: Styles.regionBackground
                    }
                }
            }
        }
    }

    Shortcut {
        id: shortcutOpenDrawer
        sequence: "Ctrl+D"
        onActivated: {
            drawer.opened ? drawer.close() : drawer.open();
        }
        context: Qt.ApplicationShortcut
        enabled: debugMode
    }

    Shortcut {
        id: shortcutFocusHost
        sequence: "Ctrl+L"
        onActivated: {
            host.focus = true
        }
    }

    Shortcut {
        sequence: "Ctrl+T"
        onActivated: {
            let telemetry = JSON.stringify({
                "chartSeriesLatencyAxisX": {
                    "min": chartSeriesLatencyAxisX.min,
                    "max": chartSeriesLatencyAxisX.max
                },
                "chartSeriesLatencyAxisY": {
                    "min": chartSeriesLatencyAxisY.min,
                    "max": chartSeriesLatencyAxisY.max
                },
                "seriesLatencyCount": seriesLatency.count,
                "packetsModelCount": packetsModel.count
            });
            addToLog(telemetry);
            backend.dumpTelemetry(`[${getCurrentDateTime()}] ${telemetry}`);
        }
        context: Qt.ApplicationShortcut
        enabled: debugMode
    }

    onClosing: {
        backend.closeEvent();
    }

    // trying to preserve whether window is maximized or not
    /*onWindowStateChanged: {
        //console.debug(settings.visibility);
        //settings.visibility = mainWindow.visibility;
        //console.debug(settings.visibility);
        //console.debug(mainWindow.visibility);
    }*/

    Rectangle {
        id: root
        anchors.fill: parent
        color: Styles.mainBackground

        RowLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            ColumnLayout {
                Layout.preferredWidth: parent.width * 0.6
                Layout.fillHeight: true
                spacing: Styles.layoutSpacing

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.1
                    Layout.minimumHeight: 50
                    Layout.maximumHeight: 70
                    border.color: backend.usingPingUtility ? Styles.colorReceived : Styles.colorError
                    border.width: backend.pinging ? 1 : 0

                    RowLayout {
                        anchors.fill: parent
                        //spacing: 15
                        anchors.leftMargin: Styles.layoutSpacing

                        FormLabelHeader {
                            //anchors.verticalCenter: parent.verticalCenter
                            //Layout.alignment: Qt.AlignBaseline
                            Layout.alignment: Qt.AlignVCenter
                            text: "Host"
                        }

                        Item
                        {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10

                            TextInput {
                                id: host
                                anchors.verticalCenter: parent.verticalCenter
                                width: parent.width
                                clip: true
                                enabled: !backend.pinging
                                font.pointSize: Styles.sectionHeaderFontSize
                                color: Styles.buttonsTextColor

                                Keys.onReturnPressed: {
                                    btn_ping.clicked();
                                }

                                // that one includes programmatical changes
                                //onTextChanged: {
                                //    console.debug(`Host text changed to: ${text}`);
                                //}

                                // that one excludes programmatical changes
                                // and reacts on every user input
                                //onTextEdited: {
                                //    console.debug(`Host text edited to: ${text}`);
                                //}

                                // that one only reacts when user submits his input
                                onEditingFinished: {
                                    //console.debug(`Host text set to: ${text}`);
                                    settings.hostname = text
                                }
                            }

                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                color: "transparent"
                                border.color: backend.usingPingUtility
                                    ? Styles.colorReceived
                                    : Styles.colorError
                                width: pingingModeLabel.contentWidth + 16
                                height: pingingModeLabel.contentHeight + 10
                                visible: backend.pinging

                                Text {
                                    id: pingingModeLabel
                                    anchors.centerIn: parent
                                    text: backend.usingPingUtility ? "ICMP" : "HTTP"
                                    font.pointSize: Styles.dialogFontSize
                                    color: backend.usingPingUtility
                                        ? Styles.colorReceived
                                        : Styles.colorError
                                }
                            }
                        }

                        HalfRoundedButton {
                            id: btn_ping
                            Layout.preferredWidth: parent.width * 0.25
                            Layout.maximumWidth: 200
                            Layout.fillHeight: true

                            text: "PING"
                            visible: !backend.pinging

                            onClicked: {
                                if (host.text.trim().length === 0)
                                {
                                    dialogNoHost.show();
                                    host.focus = true;
                                }
                                else
                                {
                                    clearPreviousData();
                                    backend.on_btn_ping_clicked(host.text);
                                    btn_report.visible = false;
                                    //btn_export.visible = false;
                                    packets.visible = true;
                                    btn_stop.focus = true;

                                    addToLog("Pinging started");
                                }
                            }
                        }

                        HalfRoundedButton {
                            id: btn_stop

                            Layout.preferredWidth: parent.width * 0.25
                            Layout.maximumWidth: 200
                            Layout.preferredHeight: parent.height

                            color: Styles.colorLost//buttonDown ? "#CC817E" : "#E57373"
                            //colorGlow: buttonDown ? "#CC817E" : "#E57373"

                            text: "STOP"
                            visible: backend.pinging

                            onClicked: {
                                addToLog("Pinging stopped");
                                btn_report.focus = true;

                                backend.on_btn_stop_clicked();
                                //loading.visible = false;
                                //loadingAnimation.running = false;
                                btn_report.visible = true;
                                //btn_export.visible = true;

                                packets.visible = false;

                                results = backend.getPingData();

                                //console.debug("Latency factor:", latencyFactor, latencyFactorAvg)

                                totalPacketsSent.text = results.Sent;
                                let percentColor = results.ReceivedPercent < 98
                                    ? results.ReceivedPercent < 95 ? Styles.colorLost : Styles.colorError
                                    : Styles.colorReceived;
                                totalPacketsReceived.text = `${results.Received} (<font color="${percentColor}">${results.ReceivedPercent}%</font>)`;
                                totalPacketsLost.text = `${results.Lost} (<font color="${percentColor}">${results.LostPercent}%</font>)`;
                                let latencyColor = results.AvgLatency >= 80 * latencyFactorAvg
                                    ? Styles.colorLost
                                    : results.AvgLatency >= 60 * latencyFactorAvg ? Styles.colorError : Styles.colorReceived;
                                totalAverageLatency.text = `<font color="${latencyColor}">${results.AvgLatency}ms</font>`;
                                conclusion.text = results.Sent < minimumRequiredPackets && !debugMode
                                    ? qsTr(`Not enough data to make a proper conclusion. Let the program to send at least ${minimumRequiredPackets} packets.`)
                                    : makeConclusion(
                                        results.LostPercent,
                                        results.AvgLatency
                                    );
                                if (settings.showReport === true) { dialogReport.show(); }
                            }
                        }

                        TapHandler {
                            onTapped: {
                                host.focus = true;
                            }
                        }
                    }
                }

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        anchors.fill: parent
                        Layout.fillHeight: true
                        anchors.margins: Styles.layoutSpacing
                        spacing: Styles.layoutSpacing

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Styles.layoutSpacing

                            FormLabelHeader {
                                text: "Time / Packets"
                            }

                            /*
                            Item { Layout.fillWidth: true }

                            FormLabel {
                                text: "⋮"
                            }
                            */
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
                                max: latencyChartWidth
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
//                            topPadding: Styles.layoutSpacing
//                            leftPadding: Styles.layoutSpacing
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

            ColumnLayout {
                Layout.preferredWidth: parent.width * 0.4
                Layout.maximumWidth: 450
                Layout.fillHeight: true
                spacing: Styles.layoutSpacing

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: Styles.layoutSpacing
                        Layout.fillHeight: true
                        spacing: Styles.layoutSpacing

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: Styles.layoutSpacing

                            FormLabelHeader {
                                id: lbl_headerPackets
                                text: "Packets"
                            }

                            Item { Layout.fillWidth: true }

                            IconButton {
                                id: btn_pieChart
                                Layout.preferredWidth: lbl_headerPackets.height
                                Layout.preferredHeight: lbl_headerPackets.height
                                imageSource: packets.visible
                                        ? "qrc:/images/pie.png"
                                        : "qrc:/images/table.png"
                                tooltipText: packets.visible
                                    ? qsTr("Show pie chart")
                                    : qsTr("Show packets list")
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
//                                    target: loading
//                                    from: 0
//                                    to: 360
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
                                text: "Status"
                            }
                            FormText {
                                anchors.right: parent.right
                                text: "Time"
                            }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.topMargin: 3
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
                                width: parent !== null ? parent.width : 0
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
                                    radius: 10
                                    samples: 20
                                    transparentBorder: true
                                    color: packetStatus.color
                                    spread: 0
                                    source: packetStatus
                                }
                                FormText {
                                    anchors.right: parent.right
                                    text: time
                                }
                            }
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: scrollToTheEnd.contentHeight + 15
                                color: Styles.mainBackground
                                opacity: 0.9
                                visible: !packets.atYEnd

                                FormLabel {
                                    id: scrollToTheEnd
                                    anchors.centerIn: parent
                                    text: "▼"
                                    font.pointSize: Styles.dialogFontSize
                                }

                                TapHandler {
                                    onTapped: packets.positionViewAtEnd();
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
                                x: -10
                                width: parent.width + 20
                                y: -10
                                height: parent.height + 20

                                margins.top: 0
                                margins.bottom: 0
                                margins.right: 0
                                margins.left: 0

                                PieSeries {
                                    id: packetsPieChartSeries
                                    size: 1.0
                                    holeSize: 0.45

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
                                transparentBorder: true
                                color: Styles.colorReceived
                                source: packetsPieChart
                                visible: pieLost.value === 0 || pieReceived.value === 0
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.topMargin: 7
                            height: 3
                            color: Styles.labelsColor
                        }

                        Row {
                            id: statsNumbers
                            Layout.topMargin: -10
                            Layout.fillWidth: true
                            layoutDirection: Qt.RightToLeft
                            spacing: 5

                            FormTextWithHover {
                                id: avgTime
                                text: "0"
                                //visible: true
                                tooltipText: qsTr("Average packets time (latency)")
                                /*TapHandler {
                                    onTapped: console.debug("ololo");
                                }*/
                            }

                            FormText { text: "|" }

                            FormTextWithHover {
                                id: percentageLost
                                text: "0%"
                                color: Styles.colorLost
                                visible: settings.showLostAsPercentage
                                tooltipText: qsTr("Percentage of lost packets")
                                TapHandler {
                                    onTapped: switchToPacketsLost(true);
                                }
                            }
                            FormTextWithHover {
                                id: packetsLost
                                text: "0"
                                color: Styles.colorLost
                                visible: !percentageLost.visible
                                tooltipText: qsTr("Packets lost")
                                TapHandler {
                                    onTapped: switchToPacketsLost(false);
                                }
                            }

                            FormText { text: "|" }

                            FormTextWithHover {
                                id: percentageReceived
                                text: "0%"
                                color: Styles.colorReceived
                                visible: settings.showReceivedAsPercentage
                                tooltipText: qsTr("Percentage of received packets")
                                TapHandler {
                                    onTapped: switchToPacketsReceived(true);
                                }
                            }
                            FormTextWithHover {
                                id: packetsReceived
                                text: "0"
                                color: Styles.colorReceived
                                visible: !percentageReceived.visible
                                tooltipText: qsTr("Packets received")
                                TapHandler {
                                    onTapped: switchToPacketsReceived(false);
                                }
                            }
                        }
                    }
                }

                LayoutRegion {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.15
                    Layout.maximumHeight: 80

                    RowLayout {
                        Layout.fillWidth: true
                        anchors.centerIn: parent
                        spacing: parent.width * 0.05 + parent.width / btn_settings.width

                        IconButton {
                            id: btn_settings
                            imageSource: "qrc:/images/settings.png"
                            tooltipText: qsTr("Open settings")
                            onClicked: { showSettingsDialog(); }
                        }
//                        IconButton {
//                            id: btn_reload
//                            imageSource: "qrc:/images/reload.png"
//                            //onClicked: { settings.makeSoundReceived = true; }
//                        }
                        IconButton {
                            id: btn_report
                            visible: false
                            imageSource: "qrc:/images/info.png"
                            tooltipText: qsTr("Show report")
                            onClicked: { dialogReport.show(); }
                        }
                        /*IconButton {
                            id: btn_export
                            visible: false
                            imageSource: "qrc:/images/export.png"
                            tooltipText: qsTr("Save report")
                        }*/
                        /*IconButton {
                            id: btn_log
                            imageSource: "qrc:/images/log.png"
                            tooltipText: qsTr("Open application log")
                            onClicked: { drawer.open(); }
                        }*/
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

    WindowMessage {
        id: dialogAbout
        windowTitle: `About ${applicationName}`
        textHeader: applicationName
        textMain: applicationVersionString
        statusImage: "/images/logo.png"
    }

    /*WindowMessage {
        id: dialogLicense
        windowTitle: "License"
        textHeader: title
        textMain: mainWindow.licensedTo.length === 0
            ? "Unregistered."
            : `<b>Registered to</b>: ${mainWindow.licensedTo}`
        statusImage: "/images/logo.png"
    }*/

    WindowMessage {
        id: dialogNoHost
        windowTitle: "No host provided"
        textHeader: title
        textMain: "In order to ping some host, you need to provide either its domain name or its IP address."
        statusImage: "/images/warning.png"
    }

    WindowDialog {
        id: dialogSettings
        windowTitle: "Settings"
        windowHeight: 400
        visible: false

        contents: ColumnLayout
        {
            anchors.fill: parent
            anchors.leftMargin: Styles.dialogPaddingLeft
            anchors.topMargin: Styles.dialogPaddingTop
            anchors.rightMargin: Styles.dialogPaddingRight
            anchors.bottomMargin: Styles.dialogPaddingBottom

            DialogText {
                Layout.bottomMargin: Styles.dialogHeaderBottomMargin
                text: "Settings"
                font.pointSize: Styles.headerFontSize
                font.bold: true
            }

            ColumnLayout {
                Layout.leftMargin: Styles.dialogPaddingLeft
                DialogText {
                    Layout.bottomMargin: Styles.dialogSubHeaderBottomMargin
                    text: "General"
                    font.pointSize: Styles.normalFontSize
                    font.bold: true
                }
                DialogSwitch {
                    id: switchUsingPingUtility
                    Layout.fillWidth: true
                    text: qsTr("using ping utility")
                    helpTooltip: "Sending ICMP requests via ping tool (ON) or just HTTP HEAD requests (OFF)"
                    checked: settings.usingPingUtility
                }
                DialogSwitch {
                    id: switchShowReport
                    Layout.fillWidth: true
                    text: qsTr("show report automatically")
                    checked: settings.showReport
                }

            }

            ColumnLayout {
                Layout.leftMargin: Styles.dialogPaddingLeft
                Layout.fillWidth: true
                Layout.topMargin: Styles.dialogSectionTopMargin

                DialogText {
                    Layout.bottomMargin: Styles.dialogSubHeaderBottomMargin
                    text: "Sounds"
                    font.pointSize: Styles.normalFontSize
                    font.bold: true
                }
                DialogSwitch {
                    id: switchSoundReceived
                    Layout.fillWidth: true
                    text: qsTr("packet received")
                    checked: settings.makeSoundReceived
                }
                DialogSwitch {
                    id: switchSoundLost
                    Layout.fillWidth: true
                    text: qsTr("packet lost")
                    checked: settings.makeSoundLost
                }
            }

            Item { Layout.fillHeight: true }

            Row {
                Layout.fillWidth: true
                layoutDirection: Qt.RightToLeft
                spacing: 5

                DialogButton {
                    text: "Save"
                    onClicked: {
                        settings.usingPingUtility = switchUsingPingUtility.checked;
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
                    }
                }
            }
        }
    }

    WindowDialog {
        id: dialogReport
        windowTitle: "Report"
        visible: false

        contents: ColumnLayout
        {
            anchors.fill: parent
            anchors.leftMargin: Styles.dialogPaddingLeft
            anchors.topMargin: Styles.dialogPaddingTop
            anchors.rightMargin: Styles.dialogPaddingRight
            anchors.bottomMargin: Styles.dialogPaddingBottom

            DialogText {
                Layout.bottomMargin: Styles.dialogHeaderBottomMargin
                text: "Report"
                font.pointSize: Styles.headerFontSize
                font.bold: true
            }

            ColumnLayout {
                Layout.fillHeight: true
                spacing: 5

                Row {
                    spacing: Styles.dialogRowSpacing
                    DialogText {
                        text: "Total packets sent:"
                    }
                    DialogText {
                        id: totalPacketsSent
                        text: "0"
                        font.bold: true
                    }
                }

                Row {
                    spacing: Styles.dialogRowSpacing
                    DialogText {
                        text: "Packets received:"
                    }
                    DialogText {
                        id: totalPacketsReceived
                        text: "0 (0%)"
                        font.bold: true
                    }
                }

                Row {
                    spacing: Styles.dialogRowSpacing
                    DialogText {
                        text: "Packets lost:"
                    }
                    DialogText {
                        id: totalPacketsLost
                        text: "0 (0%)"
                        font.bold: true
                    }
                }

                Row {
                    spacing: Styles.dialogRowSpacing
                    DialogText {
                        text: "Average latency:"
                    }
                    DialogText {
                        id: totalAverageLatency
                        text: "0ms"
                        font.bold: true
                    }
                }

//                    DialogText {
//                        Layout.topMargin: Styles.dialogHeaderBottomMargin
//                        text: "Conclusion"
//                        font.pointSize: Styles.normalFontSize
//                        font.bold: true
//                    }
                DialogText {
                    id: conclusion
                    Layout.fillWidth: true
                    Layout.topMargin: Styles.dialogHeaderBottomMargin
                    font.italic: true
                    text: "¯\\_(ツ)_/¯"
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            RowLayout {
                Layout.fillWidth: true
                //layoutDirection: Qt.RightToLeft
                spacing: 5

                DialogButton {
                    text: "Copy to clipboard"
                    onClicked: {
                        clipboard.text = [
                            `Total packets sent: %1\n`,
                            `Packets received: %2 (%3\%)\n`,
                            `Packets lost: %4 (%5\%)\n`,
                            `Average latency: %6ms\n\n`,
                            `%7`
                        ]
                        .join("")
                            .arg(results.Sent)
                            .arg(results.Received).arg(results.ReceivedPercent)
                            .arg(results.Lost).arg(results.LostPercent)
                            .arg(results.AvgLatency)
                            .arg(
                                results.Sent < minimumRequiredPackets && !debugMode
                                ? qsTr(`Not enough data to make a proper conclusion. Let the program to send at least ${minimumRequiredPackets} packets.`)
                                : makeConclusion(results.LostPercent, results.AvgLatency, true)
                            );
                    }
                }
                Item { Layout.fillWidth: true }
                DialogButton {
                    text: "Close"
                    onClicked: {
                        dialogReport.close();
                    }
                }
            }
        }
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

        chartSeriesLatencyAxisY.min = 0;
        chartSeriesLatencyAxisY.max = 100;

        totalPacketsSent.text = "0";
        totalPacketsReceived.text = "0 (0%)";
        totalPacketsLost.text = "0 (0%)";
        totalAverageLatency.text = "0ms";
        conclusion.text = "¯\\_(ツ)_/¯";

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

        settings.showLostAsPercentage = !toPackets;
    }

    function switchToPacketsReceived(toPackets)
    {
        if (toPackets === true)
            { percentageReceived.visible = false; }
        else
            { percentageReceived.visible = true; }

        settings.showReceivedAsPercentage = !toPackets;
    }

    function getCurrentDateTime()
    {
        return Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz");
    }

    function addToLog(msg)
    {
        applicationLog.append(
            `[${getCurrentDateTime()}] ${msg.trim()}`
        );
    }

    function showSettingsDialog()
    {
        switchUsingPingUtility.checked = settings.usingPingUtility;
        switchShowReport.checked = settings.showReport;
        switchSoundReceived.checked = settings.makeSoundReceived;
        switchSoundLost.checked = settings.makeSoundLost;
        dialogSettings.show();
    }

    function makeConclusion(lostPercent, averageLatency, plainText = false)
    {
//        console.log(
//            `Making conclusion with ${lostPercent}% packets lost and ${averageLatency}ms average latency`
//        );

        let lostPercentNumber = Number(lostPercent);
        let averageLatencyNumber = Number(averageLatency);

        let conclusionScore = 10;

        let conclusionReliability = "unknown";
        if (lostPercentNumber === 0)
        {
            conclusionReliability = plainText
                ? "excellent"
                : `<font color="${Styles.colorReceived}">excellent</font>`;
        }
        else if (lostPercentNumber < 0.5)
        {
            conclusionReliability = plainText
                ? "very good"
                : `<font color="${Styles.colorReceived}">very good</font>`;
        }
        else if (lostPercentNumber < 1)
        {
            conclusionReliability = plainText
                ? "quite good"
                : `<font color="${Styles.colorReceived}">quite good</font>`
        }
        else if (lostPercentNumber < 2)
        {
            conclusionReliability = plainText
                ? "good"
                : `<font color="${Styles.colorReceived}">good</font>`;
        }
        else if (lostPercentNumber < 5)
        {
            conclusionReliability = plainText
                ? "okay"
                : `<font color="${Styles.colorError}">okay</font>`;
            conclusionScore -= 2;
        }
        else if (lostPercentNumber < 10)
        {
            conclusionReliability = plainText
                ? "rather bad"
                : `<font color="${Styles.colorLost}">rather bad</font>`;
            conclusionScore -= 4;
        }
        else if (lostPercentNumber < 20)
        {
            conclusionReliability = plainText
                ? "quite bad"
                : `<font color="${Styles.colorLost}">quite bad</font>`;
            conclusionScore -= 6;
        }
        else if (lostPercentNumber < 30)
        {
            conclusionReliability = plainText
                ? "very bad"
                : `<font color="${Styles.colorLost}">very bad</font>`;
            conclusionScore -= 8;
        }
        else
        {
            conclusionReliability = plainText
                ? "outrageously horrible"
                : `<font color="${Styles.colorLost}">outrageously horrible</font>`;
            conclusionScore -= 10;
        }

        let conclusionLatency = "unknown";
        //console.debug(`Latency factor: ${latencyFactor}`);
        if (averageLatencyNumber < 15 * latencyFactor)
        {
            conclusionLatency = plainText
                ? "almost non-existent"
                : `<font color="${Styles.colorReceived}">almost non-existent</font>`;
        }
        else if (averageLatencyNumber < 30 * latencyFactor)
        {
            conclusionLatency = plainText
                ? "very low"
                : `<font color="${Styles.colorReceived}">very low</font>`;
        }
        else if (averageLatencyNumber < 40 * latencyFactor)
        {
            conclusionLatency = plainText
                ? "low"
                : `<font color="${Styles.colorReceived}">low</font>`;
            conclusionScore -= 1;
        }
        else if (averageLatencyNumber < 60 * latencyFactor)
        {
            conclusionLatency = plainText
                ? "okay"
                : `<font color="${Styles.colorError}">okay</font>`;
            conclusionScore -= 2;
        }
        else if (averageLatencyNumber < 80 * latencyFactor)
        {
            conclusionLatency = plainText
                ? "quite high"
                : `<font color="${Styles.colorLost}">quite high</font>`;
            conclusionScore -= 3;
        }
        else if (averageLatencyNumber < 100 * latencyFactor)
        {
            conclusionLatency = plainText
                ? "high"
                : `<font color="${Styles.colorLost}">high</font>`;
            conclusionScore -= 4;
        }
        else
        {
            conclusionLatency = plainText
                ? "very high"
                : `<font color="${Styles.colorLost}">very high</font>`;
            conclusionScore -= 5;
        }

        let conclusionLatencyNote = "";
        if (averageLatencyNumber > 80 * latencyFactor)
        {
            conclusionLatencyNote = " (or the target host is too far from your machine)";
        }

        let rez = plainText
        ? [
              `Your internet connection reliability is ${conclusionReliability}.`,
              `The latency is ${conclusionLatency}${conclusionLatencyNote}.`,
              `Total score: ${conclusionScore < 0 ? 0 : conclusionScore}/10.`
        ]
        : [
            `Your internet connection reliability is <b>${conclusionReliability}</b>.`,
            `The latency is <b>${conclusionLatency}</b>${conclusionLatencyNote}.`,
            `Total score: <b>${conclusionScore < 0 ? 0 : conclusionScore}/10</b>.`
        ];
        return rez.join(" ");
    }

    function getRandomHostname()
    {
        return mainWindow.hostnames[
            Math.floor(Math.random() * mainWindow.hostnames.length)
        ];
    }

    Component.onCompleted: {
        //addToLog(JSON.stringify(applicationVersion));

        if (settings.hostname.length === 0)
        {
            const randomHostname = getRandomHostname();
            console.log(`There was no hostname saved in settings, will set it to ${randomHostname}`);
            settings.hostname = randomHostname;
        }
        // hostnames shorter than 4 symbols (including dot) are already filtered out in main.cpp
        host.text = host2ping.length > 0 ? host2ping : settings.hostname;

        // start pinging right after launching
        if (host.text.trim().length !== 0 && dontStartPinging !== true) { btn_ping.clicked(); }
        else { host.focus = true; }
    }
}
