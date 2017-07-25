#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QScreen>
#include "functions.h"

// TODO menu or toolbar
// TODO settings (how much packets/latency to save, pinging timer value, debug-log on/off)
// TODO logs
// TODO some visual indicator (like a bulb) that will show the status of network connection (using analytics)
// WARNING menubar doesn't appear and also disables ⌘+Q
// TODO application icon
// TODO about windows

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO restore window geometry
    //this->move(500, 500);
    //this->resize(300, 300);

    ui->btn_stop->setVisible(false);

    // TODO better cross-scaling Windows/Linux and MacOS
    QScreen *screen = QGuiApplication::screens().at(0);
    int pdpi = static_cast<int>(screen->physicalDotsPerInch());
    int fontSize = 10;
    if (pdpi > 100) { fontSize = 14; }

    QFont font("Verdana", fontSize);
    ui->centralWidget->setFont(font);

    // TODO proper handling colors for labels
    ui->lbl_sent->setStyleSheet("QLabel { color: blue }");
    ui->lbl_received->setStyleSheet("QLabel { color: green }");
    ui->lbl_lost->setStyleSheet("QLabel { color: red }");
    ui->lbl_lostPercentage->setStyleSheet("QLabel { color: red }");

    timer.setSingleShot(false);
    connect(&timer, &QTimer::timeout,this, &MainWindow::startPing);
//    connect(&ping, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MainWindow::pinged);
    connect(&ping, QOverload<int>::of(&QProcess::finished), this, &MainWindow::pinged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pinged()
{
    // if ping process was killed, do nothing
//    qDebug() << "code:" << ping.exitCode() << "status:" << ping.exitStatus();
    if (ping.exitStatus() == 1) { return; }

    QListWidgetItem *item = new QListWidgetItem();
    effect.setSource(QUrl("qrc:/sounds/error.wav"));

//    qDebug() << ping.exitCode();

    QPair<int, QString> pckt = parsePingOutput(
                ping.exitCode(),
                ping.readAllStandardOutput()
                );

    pingData.addPacket(pckt);
    switch (pckt.first)
    {
    case 0:
        item->setText(QString("%1 | %2").arg("Packet successfully received").arg(pckt.second));
        item->setBackgroundColor(QColor("green"));
        effect.setSource(QUrl("qrc:/sounds/done.wav"));
        break;
    case 1:
        item->setText("Packet lost");
        item->setBackgroundColor(QColor("red"));
        break;
    default: // 2
//        qDebug() << rez.second;
        item->setText(pckt.second);
        item->setBackgroundColor(QColor("yellow"));
        break;
    }

    ui->lbl_sent->setText(QString::number(pingData.get_pcktSent()));
    ui->lbl_received->setText(QString::number(pingData.get_pcktReceived()));
    ui->lbl_lost->setText(QString::number(pingData.get_pcktLost()));
    QString lostPercentage = QString::number(pingData.get_lostPercentage(), 'f', 2);
    if (lostPercentage.endsWith(".00")) { lostPercentage.chop(3); }
    ui->lbl_lostPercentage->setText(QString("%1%").arg(lostPercentage));

    ui->lw_output->addItem(item);
    if (ui->lw_output->count() > pingData.get_packetsQueueSize()) { delete ui->lw_output->item(0); }
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

    pingData.resetEverything();
    ui->lw_output->clear();

    ui->lbl_sent->setText("0");
    ui->lbl_received->setText("0");
    ui->lbl_lost->setText("0");
    ui->lbl_lostPercentage->setText("0%");

    ping.setProgram("ping");
    ping.setArguments(getArgs4ping() << ui->txt_host->text());

    // TODO make the timer value a variable in config
    timer.start(1000);
}

void MainWindow::on_btn_stop_clicked()
{
    ui->btn_stop->setVisible(false);
    ui->btn_ping->setVisible(true);

    timer.stop();
    ping.kill();

//    QQueue< QPair<int, QString> > pckts = pingData.get_packets();
//    for (int i = 1; i < pckts.count(); i++)
//    {
//        qDebug() << pckts.at(i);//.first << pckts.at(i).second;
//    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // kill the ping process when MainWindow closes
    ping.kill();

    // to get rid of "unused variable" warning
    qDebug() << "Application closed:" << event;

    // TODO save window geometry
//    qDebug() << "current position:" << this->pos();
//    qDebug() << "current width:" << this->width();
//    qDebug() << "current height:" << this->height();
}
