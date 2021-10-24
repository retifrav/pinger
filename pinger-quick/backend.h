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
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QElapsedTimer>

#include "build-info.h"
#include "pingdata.h"
#include "functions.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

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
    void processPingResults(QPair<int, QString> packet);
    QJsonObject getPingData();
    int getQueueSize();
    int getQueueCount();
    // just in case overriding the close event to kill the pinging process
    void closeEvent();
    void dumpTelemetry(QString telemetry);
    QString getApplicationName();
    QJsonObject getVersionInfo();
    void showAboutQt();
    QString getLicensedTo();
    bool isUsingPingUtility();

private slots:
    void requestPingFinished(QNetworkReply *reply);

private:
    int adjustSpread(int diff);

    const QString _applicationName = "Pinger";
    bool _usingPingUtility;
    QString _currentHost;
    QProcess _ping;
    QElapsedTimer _httpRequestTimer;
    QNetworkReply *_currentPendingReply;
    bool _preflightRequestSent;
    int _lostTresholdMS;
    QTimer _timer;
    QNetworkAccessManager *_managerPing;
    QSoundEffect _effect;
    PingData _pingData;
    QSettings _settings;
    QString _telemetryFile;
    QString _licensedTo;

    const QRegularExpression _startsWithHTTP = QRegularExpression("^(http|https):\\/\\/");
};

#endif // BACKEND_H
