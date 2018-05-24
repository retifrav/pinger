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
    qmlRegisterSingletonType(QUrl("qrc:///styles.qml"), "AppStyle", 1, 0, "Styles");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) { return -1; }

    return app.exec();
}
