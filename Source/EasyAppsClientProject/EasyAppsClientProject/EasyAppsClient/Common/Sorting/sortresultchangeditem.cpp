#include "sortresultchangeditem.h"

SortResultChangedItem::SortResultChangedItem(QString item, int oldIndex, int newIndex) :
        item(item),
        oldIndex(oldIndex),
        newIndex(newIndex)
{
}

QString SortResultChangedItem::getItem() const
{
    return this->item;
}

int SortResultChangedItem::getOldIndex() const
{
    return this->oldIndex;
}

int SortResultChangedItem::getNewIndex() const
{
    return this->newIndex;
}
