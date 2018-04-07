#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "functions.h"

// TODO control the log-file size
// TODO enable/disable logging from settings
void msgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file(QDir(QDir::currentPath()).filePath("pinger.log"));
    if (file.open(QIODevice::Append))
    {
        QByteArray localMsg = msg.toLocal8Bit();
        QString dt = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
        QTextStream stream(&file);
        switch (type)
        {
        case QtDebugMsg:
            fprintf(stderr, "[debug] %s\n", localMsg.constData());
            stream << QString("[%1] [debug] %2").arg(dt).arg(msg) << endl;
            break;
        case QtInfoMsg:
            fprintf(stderr, "[info] %s\n", localMsg.constData());
            stream << QString("[%1] [info] %2").arg(dt).arg(msg) << endl;
            break;
        case QtWarningMsg:
            fprintf(stderr, "[warning] %s\n", localMsg.constData());
            stream << QString("[%1] [warning] %2").arg(dt).arg(msg) << endl;
            break;
        case QtCriticalMsg:
            fprintf(stderr, "[critical] %s\n", localMsg.constData());
            stream << QString("[%1] [critical] %2").arg(dt).arg(msg) << endl;
            break;
        case QtFatalMsg:
            fprintf(stderr, "[fatal] %s\n", localMsg.constData());
            stream << QString("[%1] [fatal] %2").arg(dt).arg(msg) << endl;
            abort();
        }
        // NOTE I'm not sure if the file gets closed in case of abort()
        file.close();
    }
    else
    {
        qDebug() << "Couldn't open the log file. Error:" << file.errorString();
    }
}

// TODO command line arguments
int main(int argc, char *argv[])
{
    // custom handler for log-messages
    qInstallMessageHandler(msgOutput);

    QApplication a(argc, argv);
    qInfo() << "Application started";

    // that is for QSettings, but it's nice to have it being set anyway
    a.setApplicationName("pinger");
    a.setOrganizationName("Wasp Enterprises");
    a.setOrganizationDomain("retifrav.github.io");

    MainWindow w;
    w.show();

    return a.exec();
}
