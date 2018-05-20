#include "backend.h"

Backend::Backend()
{
    timer.setSingleShot(false);
    connect(&timer, &QTimer::timeout,this, &Backend::startPing);
//    connect(&ping, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &Backend::pinged);
    connect(&ping, QOverload<int>::of(&QProcess::finished), this, &Backend::pinged);
}

void Backend::pinged()
{
    // if ping process was killed, do nothing
//    qDebug() << "code:" << ping.exitCode() << "status:" << ping.exitStatus();
    if (ping.exitStatus() == 1) { return; }

    effect.setSource(QUrl("qrc:/sounds/error.wav"));
    bool makeSound = true;

//    qDebug() << ping.exitCode();

    QPair<int, QString> pckt = parsePingOutput(
                ping.exitCode(),
                ping.readAllStandardOutput()
                );

    pingData.addPacket(pckt);

    QString status = "error";
    int packetColor = 2;

    switch (pckt.first)
    {
    case 0:
        status = "Received";
        packetColor = 0;
        effect.setSource(QUrl("qrc:/sounds/done.wav"));
        makeSound = false;
        break;
    case 1:
        status = "Lost";
        packetColor = 1;
        break;
    default: // 2
//        qDebug() << rez.second;
        break;
    }

    QString lostPercentage = QString("%1%")
            .arg(QString::number(pingData.get_lostPercentage(), 'f', 2));
    lostPercentage = lostPercentage.replace(".00%", "%");

    QString receivedPercentage = QString("%1%")
            .arg(QString::number(pingData.get_receivedPercentage(), 'f', 2));
    receivedPercentage = receivedPercentage.replace(".00%", "%");

    if (makeSound) { effect.play(); }

//    qDebug() << pckt.first << " | " << pckt.second;
    emit gotPingResults(
                status,
                packetColor,
                pckt.second,
                pingData.get_packetsQueueSize(),
                QString("%1 ms").arg(QString::number(pingData.get_avgTime(), 'g', 5)),
                lostPercentage,
                receivedPercentage,
                pingData.get_pcktLost(),
                pingData.get_pcktReceived(),
                pingData.get_pcktSent(),
                pingData.get_lastPacketTime()
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

    qInfo() << "Application closed";// << close;
}
