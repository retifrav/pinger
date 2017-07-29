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
//    qDebug() << "- - -\n" << pingExitCode << "|" << pingOutput << "\n- - -";

    QPair<int, QString> rez(2, "");

    QString latency, /*lost,*/ error;

    #if defined(Q_OS_WIN) // exit code is useless on Windows, we need to parse the output
        //qDebug() << "[windows]";
        if (pingExitCode == 0)
        {
            /* old version
            QRegularExpression re("time(=|<)\\d+\\w+|Lost = \\d+");
            QRegularExpressionMatchIterator i = re.globalMatch(pingOutput);
            if (i.hasNext())
            {
                latency = i.next().captured().replace("time=", "").replace("time<", "");
                //qDebug() << "latency:" << latency;
//                lost = i.next().captured().replace("Lost = ", "");
                //qDebug() << "lost:" << lost;
//                if (lost == "0") // it's always 0 here
//                {
                    rez.first = 0;
                    rez.second = latency;
//                }
//                else
//                {
//                    qDebug() << "TRULLY LOST";
//                    rez.first = 1;
//                }
            }
            */
            QRegularExpression re("(?:time(?:=|<))(\\d+\\w+)");
            QRegularExpressionMatch match = re.match(pingOutput);
            if (match.hasMatch())
            {
                latency = match.captured(1);
                //qDebug() << "latency:" << latency;
                rez.first = 0;
                rez.second = latency;
            }
            else
            {
                rez.first = 2;
                rez.second = pingOutput;
            }
        }
        else
        {
            QRegularExpression re("(?:bytes of data:\r\n)(.*)\\.\r\n");
            QRegularExpressionMatch match = re.match(pingOutput);
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
            QRegularExpression re("time=\\d+\\.\\d+ \\w+");
            QRegularExpressionMatch match = re.match(pingOutput);
            if (match.hasMatch())
            {
                latency = match.captured().replace("time=", "");
                //qDebug() << "latency:" << latency;
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

QList<int> checkWindowGeometry(
        int defaultWidth,
        int defaultHeight,
        int x2check,
        int y2check,
        int width2check,
        int height2check
        )
{
    int screenX = 0, defaultX = 0,
        screenY = 0, defaultY = 0;

    QDesktopWidget dw;
    QRect scr = dw.availableGeometry();
    screenX = scr.width();
    screenY = scr.height();

    if (width2check <= 0 || width2check > screenX) { width2check = defaultWidth; }
    defaultX = screenX / 2 - width2check / 2;
    if (height2check <= 0 || height2check > screenY) { height2check = defaultHeight; }
    defaultY = screenY / 2 - height2check / 2;

    if (x2check < 0 || x2check >= screenX) { x2check = defaultX; }
    if (y2check < 0 || y2check >= screenY) { y2check = defaultY; }

//    rez.append(x);
//    rez.append(y);
//    rez.append(width);
//    rez.append(height);
    return QList<int>() << x2check << y2check << width2check << height2check;
}
