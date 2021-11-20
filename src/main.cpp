#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFont>
#include <QScreen>
#include "backend.h"

int main(int argc, char *argv[])
{
    bool debugMode = false;

    if (argc == 2)
    {
        if (strcmp(argv[1], "--version") == 0)
        {
            QTextStream(stdout) << QString("Version: %1.%2.%3\nCommit: %4\nBuilt on: %5").arg(
                QString::number(decovar::pinger::versionMajor),
                QString::number(decovar::pinger::versionMinor),
                QString::number(decovar::pinger::versionRevision),
                QString::fromStdString(decovar::pinger::versionCommit),
                QString::fromStdString(decovar::pinger::versionDate)
            ) << Qt::endl;
            return EXIT_SUCCESS;
        }

        if (strcmp(argv[1], "--help") == 0)
        {
            QTextStream(stdout) << "Pinger - network connection quality analyzer\n"
                                << "Copyright (C) 2017, Declaration of VAR\n\n"
                                << QString("Source code: %1\n").arg(
                                       QString::fromStdString(decovar::pinger::repositoryURL)
                                   )
                                << QString("License (GPLv3): %1").arg(
                                       QString::fromStdString(decovar::pinger::licenseURL)
                                   )
                                << Qt::endl;
            return EXIT_SUCCESS;
        }

        if (strcmp(argv[1], "--debug") == 0)
        {
            debugMode = true;
        }

        // don't do that for Qt applications, they can take a lot of special Qt parameters
        //QTextStream(stdout) << "Unsupported argument" << Qt::endl;
        //return EXIT_FAILURE;
    }

    // don't do that for Qt applications, they can take a lot of special Qt parameters
    /*if (argc > 2)
    {
        QTextStream(stderr) << "Unsupported amount of arguments" << Qt::endl;
        return EXIT_FAILURE;
    }*/

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // should be QGuiApplication, but Charts depend on Graphics View
    // there is a note about this on https://doc.qt.io/qt-5/qtcharts-index.html
    QApplication app(argc, argv);

    QString appName = "Pinger";
    app.setApplicationName(appName);
    app.setApplicationDisplayName(appName);
    // don't set domain and organization name on Mac OS,
    // this should be handled by CMake, otherwise you'll get
    // a bundle identifier which you did not expect
#if !defined(Q_OS_MACOS)
    app.setOrganizationDomain("decovar.dev");
    app.setOrganizationName("Declaration of VAR"); // "dev.decovar"
#endif
    app.setApplicationVersion(
        QString("%1.%2.%3").arg(
            QString::number(decovar::pinger::versionMajor),
            QString::number(decovar::pinger::versionMinor),
            QString::number(decovar::pinger::versionRevision)
        )
    );

    // https://doc.qt.io/qt-5/scalability.html#calculating-scaling-ratio
    qreal refDpi = 216.;
    qreal refHeight = 1776.;
    qreal refWidth = 1080.;
    QRect rect = QGuiApplication::primaryScreen()->geometry();
    qreal height = qMax(rect.width(), rect.height());
    qreal width = qMin(rect.width(), rect.height());
    qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    /*auto scalingRatio = qMin(
        height / refHeight,
        width / refWidth
    );*/
    auto fontSizeRatio = qMin(
        height * refDpi / (dpi * refHeight),
        width * refDpi / (dpi * refWidth)
    );
    //qDebug() << scalingRatio << fontSizeRatio;

    QFont defaultFont("Verdana");
    app.setFont(defaultFont);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("debugMode", QVariant(debugMode));
    engine.rootContext()->setContextProperty("fontSizeRatio", QVariant(fontSizeRatio));

    qmlRegisterType<Backend>("dev.decovar.Backend", 1, 0, "Backend");
    qmlRegisterSingletonType(
        QUrl(QStringLiteral("qrc:///styles.qml")),
        "ApplicationStyles",
        1, 0,
        "Styles"
    );

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl) { QCoreApplication::exit(-1); }
        }, Qt::QueuedConnection
    );
    engine.load(url);

    return app.exec();
}
