#include "widgetlistsnapshot.h"

WidgetListSnapshot::WidgetListSnapshot()
{
}

WidgetListSnapshot::WidgetListSnapshot(QList<QWidget *> widgets)
{
    this->_setItems(widgets);
}

////
//// static factory method
//WidgetListSnapshot *WidgetListSnapshot::createSnapshotFromList(QList<QWidget *> widgets) {

//}

void WidgetListSnapshot::addItem(QWidget *item)
{
    //addedItems.push_back(item);
    this->orderedItems << item;
}

#if 0
void SortingResult::addItem(QWidget *item, int index)
{
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("implement this - addItem");
    //addedItems << item;
}
#endif

bool WidgetListSnapshot::operator==(const WidgetListSnapshot &otherSnapshot) const
{
    int thisCnt = this->getOrderedItemCount();
    int otherCnt = otherSnapshot.getOrderedItemCount();
    if(thisCnt != otherCnt) {
        DLog("Count mismatch: ") << thisCnt << otherCnt;
        return false;
    }

    bool isSame = true;
    for(int i = 0; i < thisCnt && isSame; i++) {
        if(this->orderedItems[i] != otherSnapshot.orderedItems[i]) {
            isSame = false;
            DLog("Not same: ") << this->orderedItems[i] << otherSnapshot.orderedItems[i];
        }
    }

    return isSame;
}

QList<QWidget *> WidgetListSnapshot::getOrderedItems() const
{
    return this->orderedItems;
}

void WidgetListSnapshot::_setItems(QList<QWidget *> items)
{
    this->orderedItems = items;
}

void WidgetListSnapshot::clearAll() {
    this->orderedItems.clear();
}

int WidgetListSnapshot::getOrderedItemCount() const {
    return this->orderedItems.size();
}
