#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QRegularExpression>
#include <QStringList>
#include <QDebug>

QStringList getArgs4ping();

// Time (delay/latency) is reported differently on differen OSes:
//
// - Windows: Reply from 87.250.250.242: bytes=32 time=27ms TTL=245
// - Linux: 64 bytes from ya.ru (87.250.250.242): icmp_seq=1 ttl=128 time=30.4 ms
// - Mac OS: 64 bytes from 87.250.250.242: icmp_seq=0 ttl=245 time=37.710 ms
//
// So we need to use RegEx to get numerical value. It wil be int,
// because fractions of milliseconds are of no interest
#if defined(Q_OS_WIN)
    const QRegularExpression timeRegEx = QRegularExpression("(?:time(?:=|<))(\\d+)(\\w+)");
    const QRegularExpression errorRegEx = QRegularExpression("(?:bytes of data:\r\n)(.*)\\.\r\n");
#else
    const QRegularExpression timeRegEx = QRegularExpression("(?:time=)(\\d+).* (\\w+)");
#endif
const QRegularExpression timeMSRegEx = QRegularExpression("^\\d+");

// int:
// - 0: packet received
// - 1: packet lost
// - 2: error
// QString:
// - what to display
QPair<int, QString> parsePingOutput(int pingExitCode, QString pingOutput);

int parseLatency(QString latency);

#endif // FUNCTIONS_H
