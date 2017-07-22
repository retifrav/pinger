#ifndef PINGDATA_H
#define PINGDATA_H

#include "QQueue"

class PingData
{
public:
    PingData();

    void set_pcktLost(int pl);
    void set_pcktReceive(int pr);
    void set_pcktSent(int ps);
    void addPacket(QPair<int, QString>);

    int get_pcktLost();
    int get_pcktReceived();
    int get_pcktSent();
    QQueue< QPair<int, QString> > get_packets();

    void resetEverything();

    // TODO function for generating a report to show your ISP
    //QString generateReport(int reportFormat);

private:
    int pcktLost;
    int pcktReceived;
    int pcktSent;
    QQueue< QPair<int, QString> > packets;
};

#endif // PINGDATA_H
