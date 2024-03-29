#ifndef PINGDATA_H
#define PINGDATA_H

#include "QQueue"

class PingData
{
public:
    PingData();

    //void set_packetsQueueSize(int qs);
    // TODO some analytics that would watch for anomalies (lots of packets lost or increased latency)
    void addPacket(QPair<int, QString> pckt);

    int get_pcktLost();
    int get_pcktReceived();
    int get_pcktSent();
    float get_lostPercentage();
    float get_receivedPercentage();
    int get_packetsQueueSize();
    int get_packetsQueueCount();
    QList<int> *get_packetsQueueTimes();
    float get_lastPacketTime();
    float get_avgTime();
    QQueue<QPair<int, QString>> get_packets();

    void resetEverything();

    // TODO function for generating a report to show to your ISP (txt, html, pdf)
    //QString generateReport(int reportFormat);

private:
    unsigned long pcktLost;
    unsigned long pcktReceived;
    unsigned long pcktSent;
    int packetsQueueSize;
    float avgTime;
    float lastPacketTime;
    unsigned long totalTime;
    QQueue<QPair<int, QString>> packetsQueue;

    void resizePacketsQueue(int newQueueSize);
};

#endif // PINGDATA_H
