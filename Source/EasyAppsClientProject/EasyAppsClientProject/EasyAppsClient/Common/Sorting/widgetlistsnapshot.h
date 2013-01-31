#ifndef WIDGETLISTSNAPSHOT_H
#define WIDGETLISTSNAPSHOT_H

#include <QList>
#include <QWidget>

//#include "sortresultchangeditem.h"

//
// !!! Important:
//      changedItems' indices related to the final indices -> first apply add/remove
//      and only after this apply the move / change indices !
class WidgetListSnapshot
{
public:
    explicit WidgetListSnapshot();
    explicit WidgetListSnapshot(QList<QWidget *> widgets);

    bool operator==(const WidgetListSnapshot &otherSnapshot) const;

    //
    // factory method
//    static WidgetListSnapshot *createSnapshotFromList(QList<QWidget *> widgets);

    void addItem(QWidget *item);
//    void addItem(QWidget *item, int index);

    void _setItems(QList<QWidget *> items);

    QList<QWidget *> getOrderedItems() const;

    void clearAll();
    int getOrderedItemCount() const;

private:
    QList<QWidget *> orderedItems;
};

#endif // WIDGETLISTSNAPSHOT_H
