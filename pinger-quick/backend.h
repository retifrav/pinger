#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QSoundEffect>
#include <QSettings>
#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>

#include "build_info.h"
#include "pingdata.h"
#include "functions.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    Backend();

signals:
    void gotPingResults(
        int status,
        QString time,
        //int queueSize,
        QString averageTime,
        QString lostPercentage,
        QString receivedPercentage,
        int lostPackets,
        int receivedPackets,
        int totalPackets,
        float lastPacketTime,
        int minAxisY,
        int maxAxisY
    );
    void gotError(QString errorMessage);

public slots:
    void on_btn_ping_clicked(QString host);
    void on_btn_stop_clicked();
    void pinged(int exitCode, QProcess::ExitStatus exitStatus);
    void startPing();
    QJsonObject getPingData();
    int getQueueSize();
    // just in case overriding the close event to kill the pinging process
    void closeEvent();
    void dumpTelemetry(QString telemetry);
    QString getApplicationName();
    QJsonObject getVersionInfo();
    void showAboutQt();
    QString getLicensedTo();

private:
    int adjustSpread(int diff);

    const QString _applicationName = "Pinger";
    QProcess ping;
    QTimer timer;
    QSoundEffect effect;
    PingData pingData;
    QSettings settings;
    QString _telemetryFile;

    QString _licensedTo;
};

#endif // BACKEND_H
