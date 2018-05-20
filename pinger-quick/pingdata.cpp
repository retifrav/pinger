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
    packetsQueueSize = 20;
    totalTime = 0;
    avgTime = 0;

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

float PingData::get_lastPacketTime()
{
    return lastPacketTime;
}

float PingData::get_avgTime()
{
    return avgTime;
//    qDebug() << packetsQueue.count();
//    int cnt = 0;
//    float sum = 0, tmp = 0;
//    foreach (auto pckt, packetsQueue)
//    {
//        //qDebug() << pckt.second;
//        tmp = pckt.second.replace(" ms", QString()).toFloat();
//        if (tmp != 0) { sum += tmp; cnt++; }
//    }
//    return sum / cnt;
}

QQueue<QPair<int, QString> > PingData::get_packets() { return packetsQueue; }

float PingData::get_lostPercentage()
{
    if (pcktSent == 0) { return 0; }
    return static_cast<float>(pcktLost) / static_cast<float>(pcktSent) * 100;;
}

float PingData::get_receivedPercentage()
{
    if (pcktSent == 0) { return 0; }
    return static_cast<float>(pcktReceived) / static_cast<float>(pcktSent) * 100;;
}

void PingData::addPacket(QPair<int, QString> pckt)
{
    if (pckt.first != 2)
    {
        pcktSent++;
        if (pckt.first == 0)
        {
            pcktReceived++;
            bool conversionResult = true;
            float tmp = pckt.second.replace(" ms", QString()).toFloat(&conversionResult);
            lastPacketTime = tmp;
            if (conversionResult)
            {
                totalTime += tmp;
                avgTime = totalTime / pcktReceived;
            }
        }
        else
        {
            pcktLost++;
        }
    }

    packetsQueue.enqueue(pckt);
    // in case when UI control depends on real queue size
    resizePacketsQueue(packetsQueueSize);
}
