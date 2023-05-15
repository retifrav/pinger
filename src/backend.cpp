#include "backend.h"

Backend::Backend(QObject *parent) : QObject(parent)
{
    //qDebug() << "Settings file:" << _settings.fileName();

    _pinging = false;

    _usingPingUtility = _settings.value("usingPingUtility").toBool();

    _currentHost = QString();

    _ping.setProgram("ping");

    _lostTresholdMS = 1001;

    _preflightRequestSent = false;

    _timer.setSingleShot(false);

    auto appLocalDataLocation = QDir(
                QStandardPaths::writableLocation(
                    QStandardPaths::AppLocalDataLocation
                    )
                );
    //qDebug() << "Application data:" << appLocalDataLocation.path();
    if (!appLocalDataLocation.exists())
    {
        if (!appLocalDataLocation.mkpath("."))
        {
            qWarning() << "Application data folder doesn't exist,"
                       << "and its creation failed";
        }
    }
    _telemetryFile = appLocalDataLocation.filePath("telemetry.log");

    connect(&_timer, &QTimer::timeout, this, &Backend::startPing);
    connect(
        &_ping, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        //this, &Backend::pinged
        [=](int exitCode, QProcess::ExitStatus exitStatus) { pinged(exitCode, exitStatus); }
    );

    _managerPing = new QNetworkAccessManager(this);
    // if this is enabled, don't forget to delete/deleteLater() them yourself
    //_managerPing->setAutoDeleteReplies(true);
    _managerPing->setTransferTimeout(_lostTresholdMS);
    connect(
        _managerPing, &QNetworkAccessManager::finished,
        this, &Backend::requestPingFinished
    );
    //qDebug() << "Auto delete replies:" << _managerPing->autoDeleteReplies();

    //_licensedTo = "";
}

bool Backend::isUsingPingUtility()
{
    return _usingPingUtility;
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
        { "major",      QString::number(decovar::pinger::versionMajor)         },
        { "minor",      QString::number(decovar::pinger::versionMinor)         },
        { "revision",   QString::number(decovar::pinger::versionRevision)      },
        { "commit",     QString::fromStdString(decovar::pinger::versionCommit) },
        { "date",       QString::fromStdString(decovar::pinger::versionDate)   },
        { "repository", QString::fromStdString(decovar::pinger::repositoryURL) },
        { "license",    QString::fromStdString(decovar::pinger::licenseURL)    }
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

    //qDebug() << ping.exitCode();

    QPair<int, QString> packet = parsePingOutput(
        _ping.exitCode(),
        _ping.readAllStandardOutput()
    );
    processPingResults(packet);
}

void Backend::processPingResults(QPair<int, QString> packet)
{
    _effect.setSource(QUrl("qrc:/sounds/error.wav"));
    bool makeSound = true;

    switch (packet.first)
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

    if (packet.first != 2) { _pingData.addPacket(packet); }
    else
    {
        emit gotError(packet.second);
        return;
    }

    if (packet.first != 0)
    {
        packet.second = "-";
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

    //qDebug() << packet.first << " | " << packet.second;
    emit gotPingResults(
        packet.first,
        packet.second,
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
    if (_usingPingUtility) // calling ping CLI tool to send ICMP requests
    {
        if (_ping.state() == 0)
        {
            _ping.start();
        }
        else
        {
            qDebug() << "waiting till previously started process ends...";
        }
    }
    else // sending HTTP HEAD requests
    {
        if(_httpRequestTimer.isValid())
        {
            qDebug() << "waiting till previously started process ends...";
        }
        else
        {
            auto httpRequest = QNetworkRequest(QUrl(_currentHost));
            _httpRequestTimer.start();
            if (_currentPendingReply != nullptr)
            {
                _currentPendingReply->deleteLater();
            }
            _currentPendingReply = _managerPing->head(httpRequest);
            //qDebug() << "started";
        }
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

int Backend::getQueueCount()
{
    return _pingData.get_packetsQueueCount();
}

void Backend::on_btn_ping_clicked(QString host)
{
//    ui->btn_stop->setVisible(true);
//    ui->btn_ping->setVisible(false);

    _pingData.resetEverything();
    _preflightRequestSent = false;
//    ui->lw_output->clear();

//    ui->lbl_sent->setText("0");
//    ui->lbl_received->setText("0");
//    ui->lbl_lost->setText("0");
//    ui->lbl_lostPercentage->setText("0%");

    _usingPingUtility = _settings.value("usingPingUtility").toBool();
    emit usingPingUtilityChanged();

    _currentHost = host.trimmed();
    if (_currentHost.isEmpty())
    {
        auto errorMsg = "Got an empty host, cannot ping that";
        qWarning() << errorMsg;
        emit gotError(errorMsg);
        return;
    }

    QRegularExpressionMatch match = _startsWithHTTP.match(_currentHost);
    if (_usingPingUtility)
    {
        if (match.hasMatch())
        {
            _currentHost = _currentHost.replace(_startsWithHTTP, "");
        }
    }
    else
    {
        if (!match.hasMatch())
        {
            _currentHost = _currentHost.prepend("http://");
        }
    }

    _ping.setArguments(getArgs4ping() << _currentHost);

    // TODO make the timer value a variable in config
    _timer.start(1000);

    _pinging = true;
    emit pingingChanged();
}

void Backend::on_btn_stop_clicked()
{
//    ui->btn_stop->setVisible(false);
//    ui->btn_ping->setVisible(true);

    _timer.stop();

    if (_usingPingUtility)
    {
        _ping.kill();
    }
    else
    {
        // the STOP button can be clicked before new reply object is created by timer
        if (_currentPendingReply != nullptr)
        {
            _currentPendingReply->abort();
            //qDebug() << "aborted";
        }
        _httpRequestTimer.invalidate();
    }

//    QQueue< QPair<int, QString> > pckts = pingData.get_packets();
//    for (int i = 1; i < pckts.count(); i++)
//    {
//        qDebug() << pckts.at(i);//.first << pckts.at(i).second;
//    }

    _pinging = false;
    emit pingingChanged();
}

void Backend::closeEvent()
{
    // kill the ping process when MainWindow closes
    if (_usingPingUtility) { _ping.kill(); }

    // save window geometry (if it's a Qt Widgets application)
    /*if (this->windowState() == Qt::WindowMaximized ||this->windowState() == Qt::WindowFullScreen)
    {
        settings->setValue("window/fullscreen", 1);
    }
    else
    {
        settings->setValue("window/fullscreen", 0);
        settings->setValue("window/x", this->pos().rx());
        settings->setValue("window/y", this->pos().ry());
        settings->setValue("window/width", this->width());
        settings->setValue("window/height", this->height());
    }*/

    //qDebug() << "Application closed";
}

void Backend::dumpTelemetry(QString telemetry)
{
    //qDebug() << "Telemetry file is here:" << _telemetryFile;
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

bool Backend::isPinging()
{
    return _pinging;
}

//QString Backend::getLicensedTo()
//{
//    return _licensedTo;
//}

void Backend::requestPingFinished(QNetworkReply *reply)
{
    qint64 requestTime = 0;
    if(_httpRequestTimer.isValid())
    {
        requestTime = _httpRequestTimer.elapsed();
        _httpRequestTimer.invalidate();
    }

    //qDebug() << "Current queue size:" << getQueueCount();

    //qDebug() << "Current host:" << _httpRequest->url();
    //delete _httpRequest;

    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto err = reply->error();
//    QByteArray data;
//    if (reply->isOpen())
//    {
//        data = reply->readAll();
//    }

    //qDebug() << status << "|" << err << "|" << requestTime << "ms |";//<< data;

    auto packet = QPair<int, QString>(
        0,
        QString("%1 ms").arg(QString::number(requestTime))
    );
    //qDebug() << packet;

    //qDebug() << "Packets in queue:" << getQueueCount();

    if (requestTime > _lostTresholdMS) { packet.first = 1; }
    else
    {
        if (status != 200)
        {
            QString errorMessage = "Unknown network error";
            if (err == 0)
            {
                // dictionary: http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
                errorMessage = QString("HTTP status: %1").arg(QString::number(err));
            }
            else
            {
                switch (err)
                {
                    case QNetworkReply::OperationCanceledError:
                        return;
                    case QNetworkReply::NoError:
                        packet.first = 1;
                            break;
                    default:
                        qDebug() << "Network error:" << err;
                        packet.first = 2;
                        packet.second = errorMessage;
                }
            }
        }
    }

    // first HTTP request is always the slowest, skip its processing (if it is successful)
    if (!_usingPingUtility && !_preflightRequestSent)
    {
        //qDebug() << "Kind of a pre-flight HTTP request, skipping it";
        _preflightRequestSent = true;
        if (status == 200) { return; }
    }

    processPingResults(packet);
}
