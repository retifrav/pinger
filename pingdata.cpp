#include "pingdata.h"

PingData::PingData()
{
    resetEverything();
}

void PingData::resetEverything()
{
    pcktLost = 0;
    pcktReceived = 0;
    pcktSent = 0;
    packetsQueueSize = 10;

    packetsQueue.clear();
}

void PingData::resizePacketsQueue(int newQueueSize)
{
    while (packetsQueue.count() > newQueueSize)
    {
        /*qDebug() << "deleted:" << */packetsQueue.dequeue();
        //qDebug() << "count:" << packetsQueue.count();
    }
}

void PingData::set_packetsQueueSize(int qs)
{
    packetsQueueSize = qs;
    resizePacketsQueue(packetsQueueSize);
}

int PingData::get_pcktLost() { return pcktLost; }
int PingData::get_pcktReceived() { return pcktReceived; }
int PingData::get_pcktSent() { return pcktSent; }
int PingData::get_packetsQueueSize() { return packetsQueueSize; }
QQueue<QPair<int, QString> > PingData::get_packets() { return packetsQueue; }

void PingData::addPacket(QPair<int, QString> pckt)
{
    if (pckt.first != 2)
    {
        pcktSent++;
        if(pckt.first == 0) { pcktReceived++; } else { pcktLost++; }
    }

    packetsQueue.enqueue(pckt);
    resizePacketsQueue(packetsQueueSize);
}
