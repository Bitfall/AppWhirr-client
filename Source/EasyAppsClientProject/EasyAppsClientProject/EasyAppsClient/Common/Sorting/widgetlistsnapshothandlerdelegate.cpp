#include "widgetlistsnapshothandlerdelegate.h"

WidgetListSnapshotHandlerDelegate::WidgetListSnapshotHandlerDelegate(QObject *parent) :
    QObject(parent),
    _currentItem(NULL)
{
}

QList<QWidget *> WidgetListSnapshotHandlerDelegate::getAllItems() const
{
    return this->_items;
}

int WidgetListSnapshotHandlerDelegate::getAllItemCount() const {
    return this->_items.size();
}

QWidget *WidgetListSnapshotHandlerDelegate::getCurrentItem() const
{
    return this->_currentItem;
}

QWidget *WidgetListSnapshotHandlerDelegate::getFirstItem() const
{
    if(this->_items.size() > 0) {
        return this->_items[0];
    } else {
        return NULL;
    }
}

void WidgetListSnapshotHandlerDelegate::selectNextItem()
{
    int idx = -1;

    if(this->_currentItem != NULL) {
        idx = this->_items.indexOf(this->_currentItem);
    }

    if(idx >= 0)
    {
        // deselect the current one
        //this->deselectCurrentItem();

        ++idx;
        if(this->_items.size()-1 >= idx)
        {
            this->selectThisItem(this->_items[idx]);
        }
    } else {
        // no current selected: select the first one, if there are any
        if(this->_items.size() > 0) {
            this->selectThisItem(this->_items[0]);
        }
    }
}

void WidgetListSnapshotHandlerDelegate::selectPreviousItem()
{
    int idx = -1;

    if(this->_currentItem != NULL) {
        idx = this->_items.indexOf(this->_currentItem);
    }

    if(idx >= 0)
    {
        // deselect the current one
        //this->deselectCurrentItem();

        --idx;
        if(idx >= 0)
        {
            this->selectThisItem(this->_items[idx]);
        }
    } else {
        // no current selected: select the last one, if there are any
        if(this->_items.size() > 0) {
            this->selectThisItem(this->_items[this->_items.size() - 1]);
        }
    }
}

void WidgetListSnapshotHandlerDelegate::selectFirstItem()
{
    QWidget *firstItem = this->getFirstItem();
    if(firstItem == NULL) {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("cannot get the first item");
#endif
        return;
    }

    this->selectThisItem(firstItem);
}


void WidgetListSnapshotHandlerDelegate::selectThisItem(QWidget *item)
{
    if(item == NULL) {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("selectThisItem - Item is NULL");
#endif
        return;
    }

    this->deselectCurrentItem();
    this->_currentItem = item;

    Q_EMIT itemSelected(this->_currentItem);
}

void WidgetListSnapshotHandlerDelegate::deselectThisItem(QWidget *item)
{
    if(item == NULL) {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("deselectThisItem - Item is empty");
#endif
        return;
    }

    if(item == this->_currentItem) {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("deselectThisItem - item is matches the current item");
#endif
    } else {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("deselectThisItem - !! item does NOT maches the current item");
#endif
    }

    QWidget *tmpCurrItem = this->_currentItem;
    this->_currentItem = NULL; // set it to empty item

    Q_EMIT itemDeselected(tmpCurrItem);
}

void WidgetListSnapshotHandlerDelegate::deselectCurrentItem()
{
    this->deselectThisItem(this->_currentItem);
}

// Q_SLOT
void WidgetListSnapshotHandlerDelegate::commitSnapshot(WidgetListSnapshot snapshot)
{
    if(this->_items == snapshot.getOrderedItems()) {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("No change found compared to the current snapshot, won't do anything.");
#endif
        return;
    }

#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
    DLog("Snapshot changed: ") << this->_items.size() << snapshot.getOrderedItemCount();
#endif

    WidgetListSnapshot oldSnapshot;
    oldSnapshot._setItems( this->_items );
    this->_items = snapshot.getOrderedItems();

    // invalidate the cached widget list
//    _cachedItemWidgets.clear();
//    qDebug(" i invalidate _cachedItemWidgets");

    // and Q_EMIT Q_SIGNAL about the change
    Q_EMIT snapshotChanged();
    Q_EMIT itemsChanged(oldSnapshot, snapshot);
//    Q_EMIT snapshotChanged();
}
