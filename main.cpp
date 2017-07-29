#include "mainwindow.h"
#include <QApplication>

// TODO command line arguments
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // that is for QSettings, but it's nice to have it being set anyway
    a.setApplicationName("pinger");
    a.setOrganizationName("Wasp Enterprises");
    a.setOrganizationDomain("retifrav.github.io");

    MainWindow w;
    w.show();

    return a.exec();
}
