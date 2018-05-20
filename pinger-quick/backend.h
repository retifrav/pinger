#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QSoundEffect>
#include "pingdata.h"
#include "functions.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    Backend();

signals:
    void gotPingResults(
            QString status,
            int packetColor,
            QString time,
            int queueSize,
            QString averageTime,
            QString lostPercentage,
            QString receivedPercentage,
            int lostPackets,
            int receivedPackets,
            int totalPackets,
            float lastPacketTime
            );

public slots:
    void on_btn_ping_clicked(QString host);
    void on_btn_stop_clicked();
    void pinged();
    void startPing();
    // just in case overriding the close event to kill the pinging process
    void closeEvent();

private:
    QProcess ping;
    QTimer timer;
    QSoundEffect effect;
    PingData pingData;
    //QSettings *settings;
};

#endif // BACKEND_H
