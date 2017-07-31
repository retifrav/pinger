#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QLabel>
#include <QDebug>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = 0);

signals:
    void clicked();

public slots:
    void slotClicked();

protected:
    void mousePressEvent(QMouseEvent *event);

};

#endif // CLICKABLELABEL_H
