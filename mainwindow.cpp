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

    ui->btn_stop->setVisible(false);

    // TODO better cross-scaling Windows/Linux and MacOS
    QScreen *screen = QGuiApplication::screens().at(0);
    int pdpi = static_cast<int>(screen->physicalDotsPerInch());
    int fontSize = 10;
    if (pdpi > 100) { fontSize = 14; }

    QFont font("Verdana", fontSize);
    ui->centralWidget->setFont(font);

    // TODO get to the bottom of colors for labels
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

//void MainWindow::pinged(int exitCode)
void MainWindow::pinged()
{
    // TODO move all processing outside the UI

    QListWidgetItem *item = new QListWidgetItem();
    effect.setSource(QUrl("qrc:/sounds/error.wav"));

    int exitCode = ping.exitCode();
    qDebug() << exitCode;

    // if it's Windows, then exit code is useless - we need to parse the output
    #if defined(Q_OS_WIN)
        exitCode = parsePingOutput(exitCode, ping.readAllStandardOutput());
    #endif

    switch (exitCode)
    {
    case 64:
        item->setText("The command was used incorrectly");
        item->setBackgroundColor(QColor("blue"));
        break;
    case 68:
        item->setText("Host name unknown");
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
    ui->lw_output->scrollToBottom();

    effect.play();
}

void MainWindow::startPing()
{
    if (ping.state() == 0)
    {
        ping.start();
    }
    else
    {
        qDebug() << "waiting till previously started process ends...";
    }
}

void MainWindow::on_btn_ping_clicked()
{
    ui->btn_stop->setVisible(true);
    ui->btn_ping->setVisible(false);

    ping.setProgram("ping");
    ping.setArguments(getArgs4ping() << ui->txt_host->text());

    timer.start(1001);
}

void MainWindow::on_btn_stop_clicked()
{
    ui->btn_stop->setVisible(false);
    ui->btn_ping->setVisible(true);

    timer.stop();
    ping.kill();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // kill the ping process when MainWindow closes
    ping.kill();
}
