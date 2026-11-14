#ifndef CLIPHISTITEM_H
#define CLIPHISTITEM_H

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QVector>

struct ClipboardItem
{
    qint64 itemID;
    QByteArray content;
    QString mimeType;
    QByteArray preview;
    bool pinned;
};

#endif