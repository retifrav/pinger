#include "backend.h"
#include <QApplication>

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

    int status = 2;

    switch (pckt.first)
    {
    case 0:
        status = 0;
        effect.setSource(QUrl("qrc:/sounds/done.wav"));
        makeSound = settings.value("makeSoundReceived").toBool();
        break;
    case 1:
        status = 1;
        makeSound = settings.value("makeSoundLost").toBool();
        break;
    default: // 2
//        qDebug() << rez.second;
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

    // min latency value
    QList<float> *times = pingData.get_packetsQueueTimes();

    int minLatency = (int)(*std::min_element(times->begin(), times->end())),
        maxLatency = (int)(*std::max_element(times->begin(), times->end())),
        diff = calculateAxisAdjusment(maxLatency - minLatency);

    //qDebug() << minLatency << " | " << maxLatency;

    maxLatency += diff * 0.8 - 1;
    minLatency -= diff - 5; if (minLatency < 0) { minLatency = 0; }

    delete times;

//    qDebug() << pckt.first << " | " << pckt.second;
    emit gotPingResults(
                status,
                pckt.second,
                pingData.get_packetsQueueSize(),
                //QString("%1 ms").arg(QString::number(pingData.get_avgTime(), 'g', 5)),
                QString::number(pingData.get_avgTime(), 'g', 5),
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

// TODO maybe it can somehow be a bit better than a bunch of if-returns
int Backend::calculateAxisAdjusment(int diff)
{
    if (diff < 15) { return 5; }
    if (diff < 30) { return 10; }
    if (diff < 50) { return 15; }
    if (diff < 80) { return 20; }
    if (diff < 100) { return 25; }
    if (diff < 200) { return 30; }
    if (diff < 300) { return 35; }
    if (diff < 400) { return 40; }
    if (diff < 500) { return 45; }
    if (diff < 800) { return 50; }
    return 60;
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
