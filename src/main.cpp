#include <QCommandLineParser>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFont>
#include <QScreen>
#include <QQuickStyle>

#include "backend.h"
#include "clipboardproxy.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // should be QGuiApplication, but Charts depend on Graphics View
    // there is a note about this on https://doc.qt.io/qt-5/qtcharts-index.html
    QApplication app(argc, argv);

    const QString appName = "Pinger";
    const QString applicationDescription = "Pinger - network connection quality analyzer";
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

    QCommandLineParser parser;
    parser.setApplicationDescription(applicationDescription);
    parser.addHelpOption();
    parser.addPositionalArgument(
        "host",
        QCoreApplication::translate("main", "Host to ping")
    );
    parser.addOptions({
        {
            "version",
            QCoreApplication::translate("main", "Show version information")
        },
        {
            "license",
            QCoreApplication::translate("main", "Show license information")
        },
        {
            "dont-start-pinging",
            QCoreApplication::translate("main", "Don't start pinging right after launch")
        },
        {
            "debug",
            QCoreApplication::translate("main", "Enable debug mode")
        }/*,
        {
            "host",
            QCoreApplication::translate("main", "Host to ping"),
            QCoreApplication::translate("main", "host")
        }*/
    });
    parser.process(app);

    const bool showLicense = parser.isSet("license");
    if (showLicense)
    {
        QTextStream(stdout) << QString("%1\n").arg(applicationDescription)
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

    const bool showVersion = parser.isSet("version");
    if (showVersion)
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

    const bool debugMode = parser.isSet("debug");
    const bool dontStartPinging = parser.isSet("dont-start-pinging");

    const QStringList args = parser.positionalArguments();
    QString host2ping = args.size() > 0 ? args.at(0) : "";//parser.value("host");
    if (!host2ping.isEmpty() && host2ping.length() < 4)
    {
        host2ping = "";
        qWarning() << "Provided host value is too short, it will be ignored";
    }
    //qDebug() << "Debug mode:" << debugMode << "|" << "host:" << host2ping;

    // https://doc.qt.io/qt-6/scalability.html#calculating-scaling-ratio
    qreal refWidth = 3840.0;
    qreal refHeight = 2160.0;
    qreal refDpi = 200.26; // https://www.sven.de/dpi/
    QRect rect = QGuiApplication::primaryScreen()->geometry();
    qreal height = qMax(rect.width(), rect.height());
    qreal width = qMin(rect.width(), rect.height());
    qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    auto fontSizeRatio = qMin(
        height * refDpi / (dpi * refHeight),
        width * refDpi / (dpi * refWidth)
    );

    //auto scalingRatio = qMin(
    //    height / refHeight,
    //    width / refWidth
    //);
    //qDebug() << scalingRatio;

    if (debugMode)
    {
        const QString platformName = QGuiApplication::platformName();
        const QString applicationDisplayName = QGuiApplication::applicationDisplayName();
        const QString desktopFileName = QGuiApplication::desktopFileName();
        const auto primaryScreen = QGuiApplication::primaryScreen();

        qDebug().noquote().nospace() << "[DEBUG] Application configuration:" << Qt::endl
            << "- platform plugin: " << (platformName.isEmpty()           ? "[ NOT SET ]" : platformName) << Qt::endl
            << "- primary screen: " << primaryScreen->name() << "/" << primaryScreen->name()
                                    << " | " << primaryScreen->size()
                                    << " | " << primaryScreen->devicePixelRatio() << Qt::endl
            << "- calculated font size ratio: " << fontSizeRatio << Qt::endl
            << "- layout direction: " << QGuiApplication::layoutDirection() << Qt::endl
            << "- display name: " << (applicationDisplayName.isEmpty() ? "[ NOT SET ]" : applicationDisplayName) << Qt::endl
            << "- desktop file name: " << (desktopFileName.isEmpty()        ? "[ NOT SET ]" : desktopFileName) << Qt::endl;

        //QTextStream qts(stdout);
        //qts << QString("Application configuration:\n%1\n%2\n%3\n%4").arg(
        //    QString("- platform plugin: %1").arg(platformName.isEmpty() ? "[ NOT SET ]" : platformName),
        //    QString("- display name: %1").arg(applicationDisplayName.isEmpty() ? "[ NOT SET ]" : applicationDisplayName),
        //    QString("- desktop file name: %1").arg(desktopFileName.isEmpty() ? "[ NOT SET ]" : desktopFileName),
        //    QString("- layout direction: %1").arg(layoutDirection)
        //    ) << Qt::endl << Qt::endl;
        //qts.flush();
    }

    // starting with Qt 6 one needs to specify the style
    QQuickStyle::setStyle("Basic");

    QFont defaultFont("Verdana");
    app.setFont(defaultFont);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("debugMode", QVariant(debugMode));
    engine.rootContext()->setContextProperty("dontStartPinging", QVariant(dontStartPinging));
    engine.rootContext()->setContextProperty("fontSizeRatio", QVariant(fontSizeRatio));
    engine.rootContext()->setContextProperty("host2ping", QVariant(host2ping));

    qmlRegisterType<Backend>("dev.decovar.Backend", 1, 0, "Backend");
    qmlRegisterType<ClipboardProxy>("dev.decovar.Clipboard", 1, 0, "Clipboard");

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
