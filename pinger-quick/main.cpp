#include <QApplication>
#include <QQmlApplicationEngine>
#include "backend.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    app.setOrganizationName("Declaration of VAR");
    app.setApplicationName("Pinger");
    app.setOrganizationDomain("decovar.io");

    QQmlApplicationEngine engine;
    qmlRegisterType<Backend>("io.decovar.Backend", 1, 0, "Backend");
    qmlRegisterSingletonType(
                QUrl(QStringLiteral("qrc:///styles.qml")),
                "AppStyle",
                1, 0,
                "Styles"
                );

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated,
                &app, [url](QObject *obj, const QUrl &objUrl)
                {
                    if (!obj && url == objUrl) { QCoreApplication::exit(-1); }
                }, Qt::QueuedConnection
            );
    engine.load(url);

    return app.exec();
}
