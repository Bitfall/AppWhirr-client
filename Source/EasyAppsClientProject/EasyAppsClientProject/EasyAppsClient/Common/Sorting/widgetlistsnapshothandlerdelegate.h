#ifndef WIDGETLISTSNAPSHOTHANDLERDELEGATE_H
#define WIDGETLISTSNAPSHOTHANDLERDELEGATE_H

#include <QObject>

#include "widgetlistsnapshot.h"

//
// ! parent have to be the layout which uses this delegate
//
// This class manages the sorted list.
//
// ! It stores the actual / current items, but it doesn't manage the changes of items!
class WidgetListSnapshotHandlerDelegate : public QObject
{
    Q_OBJECT
public:
    explicit WidgetListSnapshotHandlerDelegate(QObject *parent = 0);

    //
    QList<QWidget *> getAllItems() const;
    //
    int getAllItemCount() const;

    //
//    QList<QWidget *> getAllWidgets();
    //
    QWidget *getCurrentItem() const;
    //
    QWidget *getFirstItem() const;

    void selectNextItem();
    void selectPreviousItem();
    void selectFirstItem();

    void selectThisItem(QWidget *item);
    // it will invalidate the current-selected item
    void deselectThisItem(QWidget *item);
    void deselectCurrentItem();

Q_SIGNALS:
    void snapshotChanged();

    void itemDeselected(QWidget *item);
    void itemSelected(QWidget *item);

    //
    // this Q_SIGNAL will be emitted every time a change in the result happens
    void itemsChanged(WidgetListSnapshot oldSnapshot, WidgetListSnapshot newSnapshot);

#if 0
    void itemAdded(QString item);
    void itemRemoved(QString item);
    void itemIndexChanged(QString item, int oldIndex, int newIndex);
#endif

public Q_SLOTS:
    void commitSnapshot(WidgetListSnapshot snapshot);


private:
    QList<QWidget *> _items;
    QWidget *_currentItem;

//    QList<QWidget *> _cachedItemWidgets;
//    IconPresenterListDelegate* iconPresenterListDelegate;
};

#endif // WIDGETLISTSNAPSHOTHANDLERDELEGATE_H
