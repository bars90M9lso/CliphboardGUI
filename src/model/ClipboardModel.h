#ifndef CLIPBOARDMODEL_H
#define CLIPBOARDMODEL_H

#include "ClipboardItem.h"

#include <QAbstractListModel>
#include <QVector>

class ClipboardModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        // ===== Конструктор и деструктор =====
        // ===== Constructor, destructor =====
        explicit ClipboardModel(QObject *parent = nullptr);
        ~ClipboardModel() override;

        // ===== Роли модели QListView/QML =====
        // ===== Model roles for QListView/QML =====
        enum Roles 
        {
            ItemIDRole = Qt::UserRole + 1,
            ContentRole,
            MimeTypeRole,
            PreviewRole,
            PinnedRole
        };
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        ClipboardItem GetItem(qint64 itemID) const;

        void SetItems(const QVector<ClipboardItem>& newItems);
        void AddItems(const ClipboardItem& newItem);
        void RemoveItem(qint64 itemID);
        void SetPinned(qint64 itemID, bool pinned);
        void ClearHistory();

    private:
        QVector<ClipboardItem> items;
        
    signals:
};

#endif 
