#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFont>
#include "backend.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // should be QGuiApplication, but Charts depend on Graphics View
    // there is a note about this on https://doc.qt.io/qt-5/qtcharts-index.html
    QApplication app(argc, argv);

    app.setOrganizationDomain("decovar.dev");
    app.setOrganizationName("dev.decovar"); // "Declaration of VAR"
    app.setApplicationName("pinger");

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
