#include "functions.h"

QStringList getArgs4ping()
{
    #if defined(Q_OS_WIN)
        return QStringList() << "-n" << "1"
                             << "-w" << "1000";
    #else
        return QStringList() << "-c" << "1"
                             << "-t" << "1";
    #endif
}

int parsePingOutput(int pungExitCode, QString pingOutput)
{
    // TODO parse the output on Windows
    return 0;
}
