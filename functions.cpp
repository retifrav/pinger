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

QPair<int, QString> parsePingOutput(int pingExitCode, QString pingOutput)
{
    qDebug() << "- - -\n" << pingExitCode << "\n" << pingOutput << "\n- - -";

    QPair<int, QString> rez(2, "Default value");

    QString latency;
    QString lost;

    #if defined(Q_OS_WIN) // exit code is useless on Windows, we need to parse the output
        qDebug() << "[windows]";
        if (pingExitCode == 0)
        {
            QRegularExpression re("time(=|<)\\d+\\w+|Lost = \\d+");
            QRegularExpressionMatchIterator i = re.globalMatch(pingOutput);
            if (i.hasNext())
            {
                latency = i.next().captured().replace("time=", "").replace("time<", "");
                //qDebug() << "latency:" << latency;
                lost = i.next().captured().replace("Lost = ", "");
                //qDebug() << "lost:" << lost;
                if (lost == "0")
                {
                    rez.first = 0;
                    rez.second = latency;
                }
                else
                {
                    rez.first = 1;
                }
            }
        }
        else
        {
            rez.first = 2;
            rez.second = pingOutput;
        }
    #else // we can rely on exit code, no need to parse the output
        qDebug() << "[not windows]";
        switch (pingExitCode)
        {
        case 0:
        {
            rez.first = 0;
            QRegularExpression re("time=\\d+\\.\\d+ \\w+");
            QRegularExpressionMatch match = re.match("pingOutput");
            // FIXME doesn't find the RegExp match
            if (match.hasMatch())
            {
                latency = match.captured().replace("time=", "");
                qDebug() << "latency:" << latency;
                rez.second = latency;
            }
            break;
        }
        case 2:
            rez.first = 1;
            break;
        default:
            rez.first = 2;
            rez.second = pingOutput;
            break;
        }
    #endif

    return rez;
}
