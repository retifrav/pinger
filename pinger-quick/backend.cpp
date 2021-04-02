#include "backend.h"
#include <QApplication>

Backend::Backend()
{
    timer.setSingleShot(false);

    connect(&timer, &QTimer::timeout,this, &Backend::startPing);
    connect(
        &ping, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        //[=](int exitCode, QProcess::ExitStatus exitStatus){ pinged(exitCode, exitStatus); });
        this, &Backend::pinged
    );
}

void Backend::pinged(int exitCode, QProcess::ExitStatus exitStatus)
{
    //qDebug() << "Exit code:" << exitCode << " | status:" << exitStatus;

    // if ping process was killed, do nothing
    if (exitStatus != QProcess::NormalExit)
    {
        qWarning() << "Ping process did not exit normally. Code:" << exitCode
                   << "| status" << exitStatus;
        return;
    }

    effect.setSource(QUrl("qrc:/sounds/error.wav"));
    bool makeSound = true;

    //qDebug() << ping.exitCode();

    QPair<int, QString> pckt = parsePingOutput(
                ping.exitCode(),
                ping.readAllStandardOutput()
                );

    if (pckt.first != 0)
    {
        emit gotError(pckt.second);
        pckt.second = "-";
    }
    pingData.addPacket(pckt);

    switch (pckt.first)
    {
    case 0:
        effect.setSource(QUrl("qrc:/sounds/done.wav"));
        makeSound = settings.value("makeSoundReceived").toBool();
        break;
    case 1:
        makeSound = settings.value("makeSoundLost").toBool();
        break;
    default: // 2
        //qDebug() << rez.second;
        makeSound = settings.value("makeSoundLost").toBool();
        break;
    }

    QString lostPercentage = QString("%1%")
            .arg(QString::number(pingData.get_lostPercentage(), 'f', 2));
    lostPercentage = lostPercentage.replace(".00%", "%");

    QString receivedPercentage = QString("%1%")
            .arg(QString::number(pingData.get_receivedPercentage(), 'f', 2));
    receivedPercentage = receivedPercentage.replace(".00%", "%");

    if (makeSound) { effect.play(); }

    // min/max latency value
    QList<int> *times = pingData.get_packetsQueueTimes();
    int minLatency = *std::min_element(times->begin(), times->end()),
        maxLatency = *std::max_element(times->begin(), times->end());
    //qDebug() << minLatency << " | " << maxLatency;
    maxLatency += 5;
    minLatency -= 5; if (minLatency < 0) { minLatency = 0; }
    delete times;

    //qDebug() << pckt.first << " | " << pckt.second;
    emit gotPingResults(
        pckt.first,
        pckt.second,
        pingData.get_packetsQueueSize(),
        QString::number(pingData.get_avgTime(), 'g', 4),
        lostPercentage,
        receivedPercentage,
        pingData.get_pcktLost(),
        pingData.get_pcktReceived(),
        pingData.get_pcktSent(),
        pingData.get_lastPacketTime(),
        minLatency,
        maxLatency
    );
}

void Backend::startPing()
{
    if (ping.state() == 0)
    {
        ping.start();
    }
    else
    {
        qDebug() << "waiting till previously started process ends...";
    }
}

void Backend::on_btn_ping_clicked(QString host)
{
//    ui->btn_stop->setVisible(true);
//    ui->btn_ping->setVisible(false);

    pingData.resetEverything();
//    ui->lw_output->clear();

//    ui->lbl_sent->setText("0");
//    ui->lbl_received->setText("0");
//    ui->lbl_lost->setText("0");
//    ui->lbl_lostPercentage->setText("0%");

    ping.setProgram("ping");
    ping.setArguments(getArgs4ping() << host);

    // TODO make the timer value a variable in config
    timer.start(1000);
}

void Backend::on_btn_stop_clicked()
{
//    ui->btn_stop->setVisible(false);
//    ui->btn_ping->setVisible(true);

    timer.stop();
    ping.kill();

//    QQueue< QPair<int, QString> > pckts = pingData.get_packets();
//    for (int i = 1; i < pckts.count(); i++)
//    {
//        qDebug() << pckts.at(i);//.first << pckts.at(i).second;
//    }
}

void Backend::closeEvent()
{
    // kill the ping process when MainWindow closes
    ping.kill();

    // save window geometry
//    if (this->windowState() == Qt::WindowMaximized ||this->windowState() == Qt::WindowFullScreen)
//    {
//        settings->setValue("window/fullscreen", 1);
//    }
//    else
//    {
//        settings->setValue("window/fullscreen", 0);
//        settings->setValue("window/x", this->pos().rx());
//        settings->setValue("window/y", this->pos().ry());
//        settings->setValue("window/width", this->width());
//        settings->setValue("window/height", this->height());
//    }

    qInfo() << "Application closed";
}
