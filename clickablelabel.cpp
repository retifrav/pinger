#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{
//    connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

void ClickableLabel::slotClicked()
{
//    qDebug() << "clicked";
}

void ClickableLabel::mousePressEvent (QMouseEvent *event)
{
//    emit clicked();
}
