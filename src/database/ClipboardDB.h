#ifndef CLIPBOARDDB_H
#define CLIPBOARDDB_H

#include "../model/ClipboardItem.h"

#include <QSqlDatabase>
#include <QVector>
#include <QString>

class ClipboardDB
{
    public:
        explicit ClipboardDB();
        ~ClipboardDB();
        
        bool OpenDatabase();
        bool CreateTables();

        qint64 AddItem(const ClipboardItem& item);
        QVector<ClipboardItem> GetItems();

        bool DeleteItem(qint64 itemID);
        bool SetPinned(qint64 itemID, bool pinned);
        bool ClearHistory();

        bool IsReady();

    private:
        QSqlDatabase database;
        bool isReadyDB = false;
        QString databasePath;
};

#endif