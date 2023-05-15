#include "clipboardproxy.h"

ClipboardProxy::ClipboardProxy(QObject *parent)
    : QObject{parent}
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    connect(
        clipboard, &QClipboard::dataChanged,
        this, &ClipboardProxy::dataChanged
    );
    connect(
        clipboard, &QClipboard::selectionChanged,
        this, &ClipboardProxy::selectionChanged
    );
}

void ClipboardProxy::setDataText(const QString &text)
{
    QGuiApplication::clipboard()->setText(text, QClipboard::Clipboard);
}

QString ClipboardProxy::dataText() const
{
    return QGuiApplication::clipboard()->text(QClipboard::Clipboard);
}

void ClipboardProxy::setSelectionText(const QString &text)
{
    QGuiApplication::clipboard()->setText(text, QClipboard::Selection);
}

QString ClipboardProxy::selectionText() const
{
    return QGuiApplication::clipboard()->text(QClipboard::Selection);
}
