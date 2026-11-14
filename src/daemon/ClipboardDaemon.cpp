#include "ClipboardDaemon.h"
#include "../database/ClipboardDB.h"
#include "../model/ClipboardItem.h"

#include "QDebug"
#include <QString>
#include <QImage>
#include <QBuffer>
#include <QCoreApplication>

ClipboardDaemon::ClipboardDaemon() 
{
    db = new ClipboardDB();
    isReadyDB = db->IsReady();
}

QString ClipboardDaemon::GetClipboardMimeType()
{
    QStringList supportedTypes = 
    {
        "image/png",
        "text/plain;charset=utf-8",
        "text/plain"
    };

    process.start("wl-paste", {"--list-types"});

    if (!process.waitForStarted() || !process.waitForFinished()) return {};

    QStringList mimeTypes = QString::fromUtf8(process.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);

    for (const QString& type : supportedTypes)
    {
        if (mimeTypes.contains(type)) return type;
    }

    return {};
}

ClipboardItem ClipboardDaemon::GetClipboardContent()
{
    QString mimeType = GetClipboardMimeType();
    ClipboardItem item;

    if (mimeType.isEmpty())
    {
        qDebug() << "Unsupported clipboard MIME type";
        return { };
    }

    process.start("wl-paste", {"--type", mimeType});

    if (!process.waitForStarted()) 
    {
        qDebug() << "Start error:" << process.errorString();
        return { };
    }

    if (!process.waitForFinished()) 
    {
        qDebug() << "Finish error:" << process.errorString();
        return { } ;
    }

    if (process.exitCode() != 0)
    {
        qDebug() << "Error:" << process.readAllStandardError();
        qDebug() << "wl-paste failed";
        return { };
    }

    QByteArray content = process.readAllStandardOutput();
        
    if (content.isEmpty() || mimeType.isEmpty())
    {
        qDebug() << "Clipboard content is empty";
        return {};
    }

    if (mimeType == "image/png") 
    {
        QImage image = QImage::fromData(content);
        if (image.isNull())
        {
            qDebug() << "Failed to decode image";
            return {};
        }
        QImage thumbnail = image.scaled
        (
            400, 400,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        QByteArray previewData;
        QBuffer buffer(&previewData);

        buffer.open(QIODevice::WriteOnly);
        thumbnail.save(&buffer, "PNG");
        item.preview = previewData;
    }
    else item.preview = QString::fromUtf8(content).left(50).toUtf8();

    item.content = content;
    item.mimeType = mimeType;
    item.pinned = false;

    return item;
}

void ClipboardDaemon::AddItem()
{
    if (isReadyDB) 
    {
        db->AddItem(GetClipboardContent());
        qDebug() << "Added item to the database";
    }
    else qDebug() << "Database is not ready. Cannot add item.";
}

void ClipboardDaemon::StartMonitoring()
{
    QString executablePath = QCoreApplication::applicationFilePath();

    watchProcess.setProcessChannelMode(QProcess::ForwardedChannels);

    watchProcess.start("wl-paste", 
    {
        "--watch",
        executablePath,
        "--capture"
    });

    if (!watchProcess.waitForStarted())
    {
        qDebug() << "Failed to start clipboard monitoring:" << watchProcess.errorString();
        return;
    }

    qDebug() << "Clipboard monitoring started";
}
