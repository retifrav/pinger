#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QSoundEffect>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QScreen>
#include "functions.h"
#include "pingdata.h"
#include "about.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_ping_clicked();
    void pinged();
    void startPing();

    void on_btn_stop_clicked();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

protected:
    // just in case overriding the close event to kill the pinging process
     void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QProcess ping;
    QTimer timer;
    QSoundEffect effect;
    PingData pingData;
    QSettings *settings;
};

#endif // MAINWINDOW_H
