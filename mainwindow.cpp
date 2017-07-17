#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QProcess>
#include <QScreen>
#include "functions.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(550, 350);

    QScreen *screen = QGuiApplication::screens().at(0);
    int pdpi = static_cast<int>(screen->physicalDotsPerInch());

    int fontSize = 10;
    if (pdpi > 100) { fontSize = 14; }
    QFont font("Verdana", fontSize);
    ui->centralWidget->setFont(font);

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
    QStringList args = getArgs4ping() << ui->txt_host->text();

    ping.start(command, args);
    ping.waitForFinished();

    qDebug() << ping.exitCode();

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
}
