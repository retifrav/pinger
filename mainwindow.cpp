#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO menu or toolbar
// TODO settings (how much packets/latency to save, pinging timer value, debug-log on/off)
// TODO logs
// TODO some visual indicator (like a bulb) that will show the status of network connection (using analytics)
// TODO application icon
// TODO standard About Qt

// NOTE list of packets should be a narrow column just showing colors and latency values (errors should go somewhere else?)
// NOTE 3(4)-value statistics should have a very big size and be in the center part

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    qDebug() << QDir(QDir::currentPath()).filePath("config.ini");
    settings = new QSettings(QDir(QDir::currentPath()).filePath("config.ini"), QSettings::IniFormat);

    // restore window geometry
    if (settings->value("window/fullscreen").toInt() != 1)
    {
        QList<int> checkedWindowGeometry = checkWindowGeometry(
                    800,
                    500,
                    settings->value("window/x").toInt(),
                    settings->value("window/y").toInt(),
                    settings->value("window/width").toInt(),
                    settings->value("window/height").toInt()
                    );
        //qDebug() << checkedWindowGeometry;

        this->move(checkedWindowGeometry.at(0), checkedWindowGeometry.at(1));
        this->resize(checkedWindowGeometry.at(2), checkedWindowGeometry.at(3));
    }
    else { this->showMaximized(); }

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

QList<int> MainWindow::checkWindowGeometry(
        int defaultWidth,
        int defaultHeight,
        int x2check,
        int y2check,
        int width2check,
        int height2check
        )
{
    int screenX = 0, defaultX = 0,
        screenY = 0, defaultY = 0;

    QDesktopWidget dw;
    QRect scr = dw.availableGeometry();
    screenX = scr.width();
    screenY = scr.height();

    if (width2check <= 0 || width2check > screenX) { width2check = defaultWidth; }
    defaultX = screenX / 2 - width2check / 2;
    if (height2check <= 0 || height2check > screenY) { height2check = defaultHeight; }
    defaultY = screenY / 2 - height2check / 2;

    if (x2check < 0 || x2check >= screenX) { x2check = defaultX; }
    if (y2check < 0 || y2check >= screenY) { y2check = defaultY; }

//    rez.append(x);
//    rez.append(y);
//    rez.append(width);
//    rez.append(height);
    return QList<int>() << x2check << y2check << width2check << height2check;
}

void MainWindow::pinged()
{
    // if ping process was killed, do nothing
//    qDebug() << "code:" << ping.exitCode() << "status:" << ping.exitStatus();
    if (ping.exitStatus() == 1) { return; }

    QListWidgetItem *item = new QListWidgetItem();
    effect.setSource(QUrl("qrc:/sounds/error.wav"));
    bool makeSound = true;

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
//        effect.setSource(QUrl("qrc:/sounds/done.wav"));
        makeSound = false;
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

    if (makeSound) { effect.play(); }
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

    // save window geometry
    if (this->windowState() == Qt::WindowMaximized ||this->windowState() == Qt::WindowFullScreen)
    {
        settings->setValue("window/fullscreen", 1);
    }
    else
    {
        settings->setValue("window/fullscreen", 0);
        settings->setValue("window/x", this->pos().rx());
        settings->setValue("window/y", this->pos().ry());
        settings->setValue("window/width", this->width());
        settings->setValue("window/height", this->height());
    }

    // to get rid of "unused variable" warning
    qInfo() << "Application closed:" << event;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionAbout_triggered()
{
    About *about = new About(this);
    about->show();
}
