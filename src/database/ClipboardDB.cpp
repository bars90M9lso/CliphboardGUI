#include "ClipboardDB.h"
#include "../model/ClipboardItem.h"

#include <QSqlError>
#include <QDebug>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

ClipboardDB::ClipboardDB() 
{
    if (OpenDatabase())
    {   
        isReadyDB = CreateTables();
    }
}

ClipboardDB::~ClipboardDB()
{
    database.close();
    database = QSqlDatabase();
}

QString GetDatabasePath()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir().mkpath(appDataPath);

    return appDataPath + "/clipboardHistory.db";
}

bool ClipboardDB::IsReady()
{
    return isReadyDB;
}

bool ClipboardDB::OpenDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE", "ClipboardDBConnection");
    database.setDatabaseName(GetDatabasePath());

    if (!database.open())
    {
        qDebug() << "Failed to open database:" << database.lastError().text();
        return false;
    }

    return true;
}

bool ClipboardDB::CreateTables()
{
    QSqlQuery query(database);

    bool result = query.exec(R"(
        CREATE TABLE IF NOT EXISTS clipboardItems 
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            content BLOB NOT NULL,
            mimeType TEXT NOT NULL,
            preview TEXT,
            pinned INTEGER DEFAULT 0
        );
    )");

    if (!result)
    {
        qDebug() << "Failed to create tables:" << query.lastError().text();
        return false;
    }

    return true;
}

qint64 ClipboardDB::AddItem(const ClipboardItem& item)
{
    QSqlQuery query(database);

    query.prepare(R"(
        INSERT INTO clipboardItems
        (content, mimeType, preview, pinned)
        VALUES
        (:content, :mimeType, :preview, :pinned);
    )");

    query.bindValue(":content", item.content);
    query.bindValue(":mimeType", item.mimeType);
    query.bindValue(":preview", item.preview);
    query.bindValue(":pinned", item.pinned);

    if (!query.exec())
    {
        qDebug() << "Failed to  INSERT INTO clipboardItems:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toLongLong();
}

QVector<ClipboardItem> ClipboardDB::GetItems()
{
    QVector<ClipboardItem> items;
    QSqlQuery query(database);

    if (!query.exec(R"(SELECT * FROM clipboardItems)"))
    {
        qDebug() << "Failed to SELECT * FROM clipboardItems:" << query.lastError().text();
        return { };
    }

    while (query.next())
    {
        ClipboardItem item;

        qint64 itemID = query.value("id").toLongLong();
        QByteArray content = query.value("content").toByteArray();
        QString mimeType = query.value("mimeType").toString();
        QByteArray preview = query.value("preview").toByteArray();
        bool pinned = query.value("pinned").toBool();

        item.itemID = itemID;
        item.content = content;
        item.mimeType = mimeType;
        item.preview = preview;
        item.pinned = pinned;

        items.append(item);
    }

    return items;
}

bool ClipboardDB::DeleteItem(qint64 itemID)
{
    QSqlQuery query(database);

    query.prepare(R"(DELETE FROM clipboardItems WHERE id = :id)");

    query.bindValue(":id", itemID);

    if (!query.exec())
    {   
        qDebug() << "Failed to DELETE FROM clipboardItems WHERE id = :id " << query.lastError().text();
        return false;
    }   

    if (query.numRowsAffected() == 1) return true;
    else if (query.numRowsAffected() == 0 ) qDebug() << "Item not found:" << itemID;
    else qDebug() << "Failed to DELETE ";
    
    return false;
}

bool ClipboardDB::SetPinned(qint64 itemID, bool pinned)
{
    QSqlQuery query(database);

    query.prepare(R"(
        UPDATE clipboardItems
        SET pinned = :pinned
        WHERE id = :id;
    )");

    query.bindValue(":id", itemID);
    query.bindValue(":pinned", pinned);

    if (!query.exec())
    {   
        qDebug() << "Failed to UPDATE clipboardItems: " << query.lastError().text();
        return false;
    }   

    if (query.numRowsAffected() == 1) return true;
    else if (query.numRowsAffected() == 0 ) qDebug() << "Item not found:" << itemID;
    
    return false;
}

bool ClipboardDB::ClearHistory()
{
    QSqlQuery query(database);
    
    if (!query.exec(R"(DELETE FROM clipboardItems WHERE pinned = 0;)"))
    {
        qDebug() << "Failed to DELETE FROM clipboardItems WHERE pinned = 0: " << query.lastError().text();
        return false;
    }
    
    return true;
}
