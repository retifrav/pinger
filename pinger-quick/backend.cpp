#include "backend.h"

Backend::Backend(QObject* parent) : QObject(parent)
{
    _ping.setProgram("ping");

    _timer.setSingleShot(false);

    auto appLocalDataLocation = QDir(
                QStandardPaths::writableLocation(
                    QStandardPaths::AppLocalDataLocation
                    )
                );
    if (!appLocalDataLocation.exists())
    {
        if (!appLocalDataLocation.mkpath("."))
        {
            qWarning() << "Application data folder doesn't exist,"
                       << "and its creation failed";
        }
    }
    _telemetryFile = appLocalDataLocation.filePath("telemetry.log");

    connect(&_timer, &QTimer::timeout,this, &Backend::startPing);
    connect(
        &_ping, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        //this, &Backend::pinged
        [=](int exitCode, QProcess::ExitStatus exitStatus) { pinged(exitCode, exitStatus); }
    );

    _managerPing = new QNetworkAccessManager(this);
    connect(
        _managerPing, &QNetworkAccessManager::finished,
        this, &Backend::requestPingFinished
    );

    _licensedTo = "";
}

QJsonObject Backend::getVersionInfo()
{
//    return QString("Version: %1.%2.%3, commit: %4, built on: %5").arg(
//        QString::number(decovar::pinger::versionMajor),
//        QString::number(decovar::pinger::versionMinor),
//        QString::number(decovar::pinger::versionRevision),
//        QString::fromStdString(decovar::pinger::versionCommit),
//        QString::fromStdString(decovar::pinger::versionDate)
//    );
    QJsonObject version
    {
        {"major",    QString::number(decovar::pinger::versionMajor)},
        {"minor",    QString::number(decovar::pinger::versionMinor)},
        {"revision", QString::number(decovar::pinger::versionRevision)},
        {"commit",   QString::fromStdString(decovar::pinger::versionCommit)},
        {"date",     QString::fromStdString(decovar::pinger::versionDate)}
    };
    return version;
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

    _effect.setSource(QUrl("qrc:/sounds/error.wav"));
    bool makeSound = true;

    //qDebug() << ping.exitCode();

    QPair<int, QString> pckt = parsePingOutput(
        _ping.exitCode(),
        _ping.readAllStandardOutput()
    );

    switch (pckt.first)
    {
    case 0:
        _effect.setSource(QUrl("qrc:/sounds/done.wav"));
        makeSound = _settings.value("makeSoundReceived").toBool();
        break;
    case 1:
        makeSound = _settings.value("makeSoundLost").toBool();
        break;
    default: // 2
        //qDebug() << rez.second;
        makeSound = _settings.value("makeSoundLost").toBool();
        break;
    }

    if (pckt.first != 2) { _pingData.addPacket(pckt); }
    else
    {
        emit gotError(pckt.second);
        return;
    }

    if (pckt.first != 0)
    {
        pckt.second = "-";
    }

    QString lostPercentage = QString("%1%")
        .arg(QString::number(_pingData.get_lostPercentage(), 'f', 2));
    lostPercentage = lostPercentage.replace(".00%", "%");

    QString receivedPercentage = QString("%1%")
        .arg(QString::number(_pingData.get_receivedPercentage(), 'f', 2));
    receivedPercentage = receivedPercentage.replace(".00%", "%");

    if (makeSound) { _effect.play(); }

    // min/max latency value
    QList<int> *times = _pingData.get_packetsQueueTimes();
    int minLatency = *std::min_element(times->begin(), times->end()),
        maxLatency = *std::max_element(times->begin(), times->end());
    //qDebug() << minLatency << " | " << maxLatency;
    int spread = adjustSpread(maxLatency - minLatency);
    //qDebug() << "spread:" << spread;
    maxLatency += spread;//adjustSpread(maxLatency);
    minLatency -= spread;/*adjustSpread(minLatency);*/ if (minLatency < 0) { minLatency = 0; }
    delete times;

    //qDebug() << pckt.first << " | " << pckt.second;
    emit gotPingResults(
        pckt.first,
        pckt.second,
        //pingData.get_packetsQueueSize(),
        QString::number(_pingData.get_avgTime(), 'g', 4),
        lostPercentage,
        receivedPercentage,
        _pingData.get_pcktLost(),
        _pingData.get_pcktReceived(),
        _pingData.get_pcktSent(),
        _pingData.get_lastPacketTime(),
        minLatency,
        maxLatency
    );
}

int Backend::adjustSpread(int diff)
{
    if (diff > 75) { return 3; }
    if (diff > 50) { return 5; }
    if (diff > 30) { return 10; }
    if (diff > 15) { return 15; }
    return 20;
}

void Backend::startPing()
{
    if (_ping.state() == 0)
    {
        _ping.start();

        QNetworkRequest request = QNetworkRequest(QUrl("http://ya.ru"));
        _managerPing->head(request);
    }
    else
    {
        qDebug() << "waiting till previously started process ends...";
    }
}

QJsonObject Backend::getPingData()
{
    QJsonObject results
    {
        {"Sent", _pingData.get_pcktSent()},
        {"Received", _pingData.get_pcktReceived()},
        {"ReceivedPercent", QString::number(_pingData.get_receivedPercentage(), 'f', 2).replace(".00", "")},
        {"Lost", _pingData.get_pcktLost()},
        {"LostPercent", QString::number(_pingData.get_lostPercentage(), 'f', 2).replace(".00", "")},
        {"AvgLatency", QString::number(_pingData.get_avgTime(), 'f', 2).replace(".00", "")}
    };
    return results;
}

int Backend::getQueueSize()
{
    return _pingData.get_packetsQueueSize();
}

void Backend::on_btn_ping_clicked(QString host)
{
//    ui->btn_stop->setVisible(true);
//    ui->btn_ping->setVisible(false);

    _pingData.resetEverything();
//    ui->lw_output->clear();

//    ui->lbl_sent->setText("0");
//    ui->lbl_received->setText("0");
//    ui->lbl_lost->setText("0");
//    ui->lbl_lostPercentage->setText("0%");

    _ping.setArguments(getArgs4ping() << host);

    // TODO make the timer value a variable in config
    _timer.start(1000);
}

void Backend::on_btn_stop_clicked()
{
//    ui->btn_stop->setVisible(false);
//    ui->btn_ping->setVisible(true);

    _timer.stop();
    _ping.kill();

//    QQueue< QPair<int, QString> > pckts = pingData.get_packets();
//    for (int i = 1; i < pckts.count(); i++)
//    {
//        qDebug() << pckts.at(i);//.first << pckts.at(i).second;
//    }
}

void Backend::closeEvent()
{
    // kill the ping process when MainWindow closes
    _ping.kill();

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

void Backend::dumpTelemetry(QString telemetry)
{
    qDebug() << "Telemetry file is here:" << _telemetryFile;
    QFile file(_telemetryFile);

    if (!file.open(QIODevice::Append))
    {
        qWarning() << "Couldn't open telemetry file to write new data."
                   << file.errorString();
        return;
    }

    QTextStream stream(&file);
    stream << telemetry << Qt::endl;
    stream.flush();

    file.close();
}

QString Backend::getApplicationName()
{
    return _applicationName;
}

void Backend::showAboutQt()
{
    QApplication::aboutQt();
}

QString Backend::getLicensedTo()
{
    return _licensedTo;
}

void Backend::requestPingFinished(QNetworkReply *reply)
{
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray data = reply->readAll();

    qDebug() << status << "|" << data;

    if (status != 200)
    {
        QString errorMessage = data;
        QNetworkReply::NetworkError err = reply->error();
        if (status == 0) {
            // dictionary: http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
            errorMessage = QString("QNetworkReply::NetworkError code: %1").arg(QString::number(err));
        }

        emit gotError(QString("Code %1 | %2").arg(status).arg(errorMessage));
        return;
    }
}
