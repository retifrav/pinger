#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
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

    timer.setSingleShot(false);
    connect(&timer, &QTimer::timeout,this, &MainWindow::startPing);
//    connect(&ping, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MainWindow::pinged);
    connect(&ping, QOverload<int>::of(&QProcess::finished), this, &MainWindow::pinged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pinged(int exitCode)
{
    QListWidgetItem *item = new QListWidgetItem();
    effect.setSource(QUrl("qrc:/sounds/error.wav"));
    switch (exitCode)
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
        item->setText("Packet successfully received");
        item->setBackgroundColor(QColor("green"));
        effect.setSource(QUrl("qrc:/sounds/done.wav"));
        break;
    default:
        item->setText("Something else");
        item->setBackgroundColor(QColor("blue"));
        break;
    }
    ui->lw_output->addItem(item);
    effect.play();
}

void MainWindow::startPing()
{
    QString command("ping");
    QStringList args = getArgs4ping() << ui->txt_host->text();

    ping.start(command, args);
//    qDebug() << ping.exitCode();
}

void MainWindow::on_btn_ping_clicked()
{
    timer.start(1001);
}
