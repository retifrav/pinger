#ifndef CLIPBOARDPROXY_H
#define CLIPBOARDPROXY_H

#include <QObject>
#include <QGuiApplication>
#include <QClipboard>

class ClipboardProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ dataText WRITE setDataText NOTIFY dataChanged)
    Q_PROPERTY(QString selectionText READ selectionText WRITE setSelectionText NOTIFY selectionChanged)

public:
    explicit ClipboardProxy(QObject *parent = nullptr);

    void setDataText(const QString &text);
    QString dataText() const;

    void setSelectionText(const QString &text);
    QString selectionText() const;

signals:
    void dataChanged();
    void selectionChanged();

signals:

};

#endif // CLIPBOARDPROXY_H
