#ifndef SORTRESULTCHANGEDITEM_H
#define SORTRESULTCHANGEDITEM_H

#include <QString>

class SortResultChangedItem
{
public:
    SortResultChangedItem(QString item, int oldIndex, int newIndex);

    QString getItem() const;
    int getOldIndex() const;
    int getNewIndex() const;

private:
    QString item;
    int oldIndex;
    int newIndex;

};

#endif // SORTRESULTCHANGEDITEM_H
