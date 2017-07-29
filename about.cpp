#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    // NOTE check window size on Windows (lol)
    // FIXME after right-clicking on a link it is possible to maximize a window
    ui->setupUi(this);

    ui->lbl_qtVersion->setText(QString("<b>Qt version:</b><br/>%1").arg(QT_VERSION_STR));
    ui->lbl_name->setText(QString("<font size=\"5\"><b>pinger</b></font><br><font size=\"2\">%1</font>").arg(QDateTime::currentDateTime().toString("yyyyMMdd.hhmm")));
}

About::~About()
{
    delete ui;
}
