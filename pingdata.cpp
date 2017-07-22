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

    packets.clear();
}
