#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QSoundEffect>

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
    void pinged(int exitCode);
    void startPing();

private:
    Ui::MainWindow *ui;
    QProcess ping;
    QTimer timer;
    QSoundEffect effect;
};

#endif // MAINWINDOW_H
