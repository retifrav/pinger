#ifndef PINGDATA_H
#define PINGDATA_H

#include "QQueue"
#include "QDebug"

class PingData
{
public:
    PingData();

    void set_packetsQueueSize(int qs);
    // TODO some analytics that would watch for anomalies (lots of packets lost or increased latency)
    void addPacket(QPair<int, QString> pckt);

    int get_pcktLost();
    int get_pcktReceived();
    int get_pcktSent();
    double get_lostPercentage();
    int get_packetsQueueSize();
    QQueue< QPair<int, QString> > get_packets();

    void resetEverything();

    // TODO function for generating a report to show to your ISP (txt, html, pdf)
    //QString generateReport(int reportFormat);

private:
    int pcktLost;
    int pcktReceived;
    int pcktSent;
    int packetsQueueSize;
    QQueue< QPair<int, QString> > packetsQueue;

    void resizePacketsQueue(int newQueueSize);
};

#endif // PINGDATA_H
