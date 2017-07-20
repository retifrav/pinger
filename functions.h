#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QStringList>

QStringList getArgs4ping();
int parsePingOutput(int pungExitCode, QString pingOutput);

#endif // FUNCTIONS_H
