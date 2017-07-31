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

// list:
//  (0) - x
//  (1) - y
//  (2) - width
//  (3) - height
QList<int> checkWindowGeometry(
        int defaultWidth,
        int defaultHeight,
        int x2check,
        int y2check,
        int width2check,
        int height2check
        );

#endif // FUNCTIONS_H
