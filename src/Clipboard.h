#ifndef CLIPHIST_H
#define CLIPHIST_H

#include "database/ClipboardDB.h"
#include "model/ClipboardModel.h"

#include <QObject>
#include <QVector>

class Clipboard : public QObject
{
    Q_OBJECT

    public:
        // ===== Конструктор и деструктор =====
        // ===== Constructor, destructor =====
        explicit Clipboard(QObject *parent = nullptr);
        ~Clipboard() override;

        ClipboardModel* GetModel() const;

        void SetItems();
        
        Q_INVOKABLE void CopyItem(qint64 itemID);
        Q_INVOKABLE void RemoveItem(qint64 itemID);
        Q_INVOKABLE void SetPinned(qint64 itemID, bool pinned);
        Q_INVOKABLE void ClearHistory();

        Q_PROPERTY(ClipboardModel* model READ GetModel CONSTANT)
        
    private:
        ClipboardModel* model;
        ClipboardDB* db;
    
    signals:
};

#endif // CLIPHIST_H