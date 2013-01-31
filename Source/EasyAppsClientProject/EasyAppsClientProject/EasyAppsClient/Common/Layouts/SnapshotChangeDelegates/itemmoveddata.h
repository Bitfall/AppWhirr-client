#ifndef ITEMMOVEDDATA_H
#define ITEMMOVEDDATA_H

#include <QWidget>

class ItemMovedData
{
public:
    ItemMovedData(QWidget *item, int oldIndex, int newIndex) :
            item(item),
            oldIndex(oldIndex),
            newIndex(newIndex)
    {

    }

    QWidget *getItem() const { return this->item; }
    int getOldIndex() const { return this->oldIndex; }
    int getNewIndex() const { return this->newIndex; }

private:
    QWidget *item;
    int oldIndex;
    int newIndex;
};

#endif // ITEMMOVEDDATA_H
