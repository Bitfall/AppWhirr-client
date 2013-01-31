#include "referencecountingidbasedcollection.h"

#include <QWidget>

ReferenceCountingIdBasedCollection::ReferenceCountingIdBasedCollection(bool isFreeUpRemainingItemsWhenDestructorCalled, QObject *parent) :
    QObject(parent),
    _isFreeUpRemainingItemsWhenDestructorCalled(isFreeUpRemainingItemsWhenDestructorCalled)
{
}

ReferenceCountingIdBasedCollection::~ReferenceCountingIdBasedCollection()
{
    if(_isFreeUpRemainingItemsWhenDestructorCalled)
    {
        QMapIterator<QString, ReferenceCountedItem *> i(_items);
        while(i.hasNext()) {
            i.next();
            this->_safeDeleteItem(i.value());
        }
    }
}

// ------------------------------
// --- main interface methods ---

void ReferenceCountingIdBasedCollection::addItem(IdBasedItemInterface *item)
{
    _refCountGuardMutex.lock();
    if(_items.contains(item->getId())) {
        WLog("Item with this itemId already added, won't add the new one: ") << item->getId();
    }
    else {
        _items[item->getId()] = new ReferenceCountingIdBasedCollection::ReferenceCountedItem(item);
    }
    _refCountGuardMutex.unlock();
}

// slot
void ReferenceCountingIdBasedCollection::retainItem(QString itemId)
{
    _refCountGuardMutex.lock();
    this->_retainItemWithoutGuard(itemId);
    _refCountGuardMutex.unlock();
}

// slot
void ReferenceCountingIdBasedCollection::releaseItem(QString itemId)
{
    _refCountGuardMutex.lock();
    ReferenceCountingIdBasedCollection::ReferenceCountedItem *relatedRefCountedItem = _items.value(itemId, NULL);
    if(relatedRefCountedItem == NULL) {
        WLog("Item not found for itemId: ") << itemId;
    }
    else {
        quint32 currRefCount = relatedRefCountedItem->_decrementRefCount();
        DLog("--release - Curr ref count for item: ") << itemId << currRefCount;
        if(currRefCount <= 0) {
            // ref-count reached 0, free it up
            this->_safeDeleteItem(relatedRefCountedItem);
            // and remove from the collection
            _items.remove(itemId);
        }
    }
    _refCountGuardMutex.unlock();
}

IdBasedItemInterface *ReferenceCountingIdBasedCollection::getItem(QString itemId, bool isRetainIt)
{
    IdBasedItemInterface *_ret = NULL;

    _refCountGuardMutex.lock();
    ReferenceCountingIdBasedCollection::ReferenceCountedItem *relatedRefCountedItem = _items.value(itemId, NULL);
    if(relatedRefCountedItem == NULL) {
        WLog("Item with this itemId not found: ") << itemId;
    }
    else {
        _ret = relatedRefCountedItem->getItem();
        if(isRetainIt) {
            this->_retainItemWithoutGuard(itemId);
        }
    }
    _refCountGuardMutex.unlock();

    return _ret;
}


// ------------------------
// --- internal methods ---

void ReferenceCountingIdBasedCollection::_retainItemWithoutGuard(QString itemId)
{
    ReferenceCountingIdBasedCollection::ReferenceCountedItem *relatedRefCountedItem = _items.value(itemId, NULL);
    if(relatedRefCountedItem == NULL) {
        WLog("Item not found for itemId: ") << itemId;
    }
    else {
        quint32 currRefCount = relatedRefCountedItem->_incrementRefCount();
        DLog("--retain - Curr ref count for item: ") << itemId << currRefCount;
    }
}

void ReferenceCountingIdBasedCollection::_safeDeleteItem(ReferenceCountingIdBasedCollection::ReferenceCountedItem *relatedRefCountedItem)
{
    DLog("_safeDeleteItemAndRemoveFromCollection") << relatedRefCountedItem->getItemId();
    IdBasedItemInterface *relatedItem = relatedRefCountedItem->getItem();

    //
    // note: somehow a direct QWidget cast is required - the QObject cast is not enough for QWidgets and will result in ASSERT if not casted to QWidget
    QWidget *itemCastedToQWidget = dynamic_cast<QWidget *>(relatedItem);
    if(itemCastedToQWidget != NULL) {
        AW_QOBJECT_SAFE_DELETE(itemCastedToQWidget);
    }
    else
    {
        QObject *itemCastedToQObject = dynamic_cast<QObject *>(relatedItem);
        if(itemCastedToQObject != NULL) {
            AW_QOBJECT_SAFE_DELETE(itemCastedToQObject);
        }
        else {
            AW_SAFE_DELETE(relatedItem);
        }
    }

    // delete the ref-counted item as well
    delete relatedRefCountedItem;
}
