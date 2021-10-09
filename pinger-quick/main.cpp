#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFont>
#include "backend.h"

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "--version") == 0)
        {
            QTextStream(stdout) << QString("Version: %1.%2.%3, commit: %4, built on: %5").arg(
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
            QTextStream(stdout) << QString("There is no help information yet. %1").arg(
                "Although, you can check the application version with --version"
            ) << Qt::endl;
            return EXIT_SUCCESS;
        }

        QTextStream(stdout) << "Unsupported argument" << Qt::endl;
        return EXIT_FAILURE;
    }

    if (argc > 2)
    {
        QTextStream(stderr) << "Unsupported amount of arguments" << Qt::endl;
        return EXIT_FAILURE;
    }

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

    QFont defaultFont("Verdana", 14);
    app.setFont(defaultFont);

    QQmlApplicationEngine engine;

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
