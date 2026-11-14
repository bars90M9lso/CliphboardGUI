#include "database/ClipboardDB.h"
#include "model/ClipboardModel.h"
#include "Clipboard.h"

#include "QDebug"
#include "QProcess"
#include "QString"
#include "QVector"


// ============================================================================
// Блок: Конструктор, деструктор 
// Block: Constructor, destructor
// ============================================================================
Clipboard::Clipboard(QObject *parent) : QObject{parent}
{
    db = new ClipboardDB();
    model = new ClipboardModel(this);
    if (db->IsReady()) model->SetItems(db->GetItems());
    
}

Clipboard::~Clipboard()
{
    delete db;
}


ClipboardModel* Clipboard::GetModel() const
{
    return model;
}


void Clipboard::RemoveItem(qint64 itemID)
{
    if (db->DeleteItem(itemID)) model->RemoveItem(itemID);
}

void Clipboard::SetPinned(qint64 itemID, bool pinned)
{
    if (db->SetPinned(itemID, pinned)) model->SetPinned(itemID, pinned);
}

void Clipboard::ClearHistory()
{
    if (db->ClearHistory()) model->ClearHistory();
}

void Clipboard::CopyItem(qint64 itemID)
{
    ClipboardItem item = model->GetItem(itemID);

    if (item.itemID != itemID)
    {
        qDebug() << "Clipboard item not found:" << itemID;
        return;
    }

    QProcess process;
    QStringList arguments;

    arguments << "--type" << item.mimeType;

    process.start("wl-copy", arguments);

    if (!process.waitForStarted())
    {
        qDebug() << "Failed to start wl-copy:" << process.errorString();
        return;
    }

    process.write(item.content);
    process.closeWriteChannel();

    if (!process.waitForFinished())
    {
        qDebug() << "wl-copy error:" << process.errorString();
        return;
    }
}