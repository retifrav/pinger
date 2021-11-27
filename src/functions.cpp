#include "functions.h"

QStringList getArgs4ping()
{
    #if defined(Q_OS_WIN)
        return QStringList() << "-n" << "1"
                             << "-w" << "1000";
    #elif defined(Q_OS_MACOS)
        return QStringList() << "-c" << "1"
                             << "-t" << "1";
    #else // GNU/Linux
        return QStringList() << "-c" << "1"
                             << "-W" << "1";
    #endif
}

QPair<int, QString> parsePingOutput(int pingExitCode, QString pingOutput)
{
    //qDebug() << "- - -\n" << pingExitCode << "|" << pingOutput << "\n- - -";

    QPair<int, QString> rez(2, "Unknown error");

    QString error;//latency, lost;

    #if defined(Q_OS_WIN) // exit code is useless on Windows, we need to parse the output
        //qDebug() << "[windows]";
        if (pingExitCode == 0)
        {
            QRegularExpressionMatch match = timeRegEx.match(pingOutput);
            if (match.hasMatch())
            {
                rez.first = 0;
                rez.second = QString("%1 %2").arg(
                    match.captured(1),
                    match.captured(2)
                );
            }
            else
            {
                rez.first = 2;
                rez.second = pingOutput;
            }
        }
        else
        {
            QRegularExpressionMatch match = errorRegEx.match(pingOutput);
            if (match.hasMatch())
            {
                error = match.captured(1);
                qDebug() << "error:" << error;
            }
            else { error = pingOutput; }

            if (error == "Request timed out")
            {
                rez.first = 1;
            }
            else
            {
                rez.first = 2;
                rez.second = error;
            }
        }
    #else // we can rely on exit code, no need to parse the output
        //qDebug() << "[not windows]";
        switch (pingExitCode)
        {
        case 0:
        {
            rez.first = 0;
            QRegularExpressionMatch match = timeRegEx.match(pingOutput);
            if (match.hasMatch())
            {
                rez.second = QString("%1 %2").arg(
                    match.captured(1),
                    match.captured(2)
                );
            }
            else
            {
                rez.second = "0 ms";
            }
            break;
        }
        case 2:
            rez.first = 1;
            break;
        default:
            rez.first = 2;
            rez.second = pingOutput.trimmed().isEmpty()
                ? "Unknown network error"
                : pingOutput.trimmed();
            break;
        }
    #endif

    return rez;
}

int parseLatency(QString latencyStr)
{
    QRegularExpressionMatch match = timeMSRegEx.match(latencyStr);
    if (match.hasMatch())
    {
        //qDebug() << timeMSRegEx.cap(0);
        return match.captured(0).toInt();
    }
    else { return 0; }
}
