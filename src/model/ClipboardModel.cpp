#include "ClipboardModel.h"

#include "QDebug"

#include "QString"
#include "QVector"
#include <QDebug>

// ============================================================================
// Блок: Конструктор, деструктор 
// Block: Constructor, destructor
// ============================================================================
ClipboardModel::ClipboardModel(QObject *parent): QAbstractListModel{parent}
{
    
}

ClipboardModel::~ClipboardModel()
{

}


// ============================================================================
// Блок: модель данных для QML / QListModel
// Block: data model for QML / QListModel
// ============================================================================
int ClipboardModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return items.size();
}

QVariant ClipboardModel::data(const QModelIndex &index, int role) const
{   
    if (!index.isValid() || index.row() < 0 || index.row() >= items.size()) return {};

    const ClipboardItem &item = items.at(index.row());

    switch (role) 
    {
        case ItemIDRole:  return item.itemID;
        case ContentRole:  return item.content;
        case MimeTypeRole: return item.mimeType;
        case PinnedRole: return item.pinned;
        case PreviewRole:
        {
            if (item.mimeType == "image/png")
            {
                return "data:image/png;base64," + item.preview.toBase64();
            }

            return QString::fromUtf8(item.preview);
        }
    }

    return {};
}

QHash<int, QByteArray> ClipboardModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[ItemIDRole] = "ItemID";
    roles[ContentRole] = "Content";
    roles[MimeTypeRole] = "MimeType";
    roles[PreviewRole] = "Preview";
    roles[PinnedRole] = "Pinned";

    return roles;
}


// ============================================================================
// Блок: Методы для управления данными модели
// Block: Methods for managing model data
// ============================================================================
ClipboardItem ClipboardModel::GetItem(qint64 itemID) const
{
    for (const auto &item : items) 
    {
        if (item.itemID == itemID) 
        {
            return item;
        }
    }
    return {};
}

void ClipboardModel::SetItems(const QVector<ClipboardItem>& newItems)
{
    beginResetModel();
    this->items = newItems;
    endResetModel();
}

void ClipboardModel::AddItems(const ClipboardItem& newItem)
{
    beginInsertRows(QModelIndex(), 0, 0);
    this->items.prepend(newItem);
    endInsertRows();
}

void ClipboardModel::RemoveItem(qint64 itemID)
{
    int index = -1;
    for (int i = 0; i < items.size(); ++i) 
    {
        if (this->items[i].itemID == itemID) 
        {
            index = i;
            break;
        }
    }
    if (index == -1) return;

    beginRemoveRows(QModelIndex(), index, index);
    items.remove(index);
    endRemoveRows();
}

void ClipboardModel::SetPinned(qint64 itemID, bool pinned)
{
    int index = -1;
    for (int i = 0; i < items.size(); ++i) 
    {
        if (this->items[i].itemID == itemID) 
        {
            index = i;
            break;
        }
    }
    if (index == -1) return;

    items[index].pinned = pinned;
    emit dataChanged(this->index(index), this->index(index), {PinnedRole});
}

void ClipboardModel::ClearHistory()
{
    for (int i = items.size() - 1; i >= 0; --i) 
    {
        if (this->items[i].pinned == false) 
        {
            beginRemoveRows(QModelIndex(), i, i);
            items.remove(i);
            endRemoveRows();
        }
    }
}