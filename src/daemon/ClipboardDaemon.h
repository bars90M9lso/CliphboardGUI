#ifndef CLIPBOARDDAEMON_H
#define CLIPBOARDDAEMON_H

#include "../database/ClipboardDB.h"
#include "../model/ClipboardItem.h"

#include "QString"
#include "QProcess"

class ClipboardDaemon
{
    public:
        explicit ClipboardDaemon();
        void AddItem();
        void StartMonitoring();

    private:
        ClipboardDB* db;
        QProcess process;
        QProcess watchProcess;
        bool isReadyDB = false;
        //QByteArray lastContent;

        QString GetClipboardMimeType();
        ClipboardItem GetClipboardContent();
        //void ProcessClipboard();
};

#endif