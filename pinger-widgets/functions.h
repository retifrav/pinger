#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QRegularExpression>
#include <QDesktopWidget>
#include <QStringList>
#include <QDebug>

QStringList getArgs4ping();

const char* string2char(QString str);

// <int> in QPair:
//  - 0: packet received
//  - 1: packet lost
//  - 2: error
// QString: what to display
QPair<int, QString> parsePingOutput(int pingExitCode, QString pingOutput);


#endif // FUNCTIONS_H
