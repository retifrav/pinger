#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lbl_lost->setStyleSheet("QLabel { color: red }");
    ui->lbl_received->setStyleSheet("QLabel { color: green }");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_ping_clicked()
{
    QProcess ping;
    QString command("ping");
    QStringList args = QStringList() << "-c" << "1" << "-t" << "1" << ui->txt_host->text();
    int x = 5;
    while (x > 0)
    {
        ping.start(command, args);
        ping.waitForFinished();

        QListWidgetItem *item = new QListWidgetItem();
        switch (ping.exitCode())
        {
        case 68:
            item->setText("Unknown host");
            item->setBackgroundColor(QColor("yellow"));
            break;
        case 2:
            item->setText("Packet lost");
            item->setBackgroundColor(QColor("red"));
            break;
        case 0:
            item->setText("ololo");
            item->setBackgroundColor(QColor("green"));
            break;
        default:
            item->setText("Something else");
            item->setBackgroundColor(QColor("blue"));
            break;
        }
        ui->lw_output->addItem(item);
        x--;
    }
}
