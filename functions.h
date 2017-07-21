#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QRegularExpression>
#include <QStringList>
#include <QDebug>
//#include <QPair>

QStringList getArgs4ping();

// int:
//  - 0: packet received
//  - 1: packet lost
//  - 2: error
// QString: what to display
QPair<int, QString> parsePingOutput(int pingExitCode, QString pingOutput);

#endif // FUNCTIONS_H
