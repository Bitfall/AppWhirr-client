#ifndef REFERENCECOUNTINGIDBASEDCOLLECTION_H
#define REFERENCECOUNTINGIDBASEDCOLLECTION_H

#include <QObject>
#include <QMap>
#include <QMutex>

#include "Common/idbasediteminterface.h"

/*! Implements an intelligent IdBasedWidget cache.

    The purpose of this class is to provide an IdBasedWidget cache / collection where the added items have a related 'reference count' and will be released if this count reaches 0.\n
    This makes easier the memory management of shared widgets. An example is the animation system, where usually multiple object uses the same widget. It can be used by the layout contains and presents the layout and can be used by the animation-system as well. In this case the widget can be released only when it's not used in both the layout and in the animation-system.
*/
class ReferenceCountingIdBasedCollection : public QObject
{
    Q_OBJECT

private:
    class ReferenceCountedItem
    {
    public:
        ReferenceCountedItem(IdBasedItemInterface *item) : _item(item), _refCount(1) {}

        QString getItemId() const { return _item->getId(); }
        IdBasedItemInterface *getItem() const { return _item; }

        quint32 _incrementRefCount() { ++_refCount; return _refCount; }
        quint32 _decrementRefCount() { --_refCount; return _refCount; }
        quint32 _getRefCount() const { return _refCount; }

    private:
        IdBasedItemInterface *_item;
        quint32 _refCount;
    };


public:
    /*!
        @param isFreeUpRemainingItemsWhenDestructorCalled : with this you can manage whether the collection should free up the remaining items' memory when destructor called or not. If you have the items' ownership then you can turn this on (set \b true ) and it will manage the items' memory automatically. But if you have to use Qt's parenting mechanism then you should set this to \b false . For example: if you add widgets as items and the widgets are part of a layout then Qt will free them up when the layout's destructor (actually the layout's widget's destructor) is called, so in this case set this to \b false to prevent double deallocation.

        @note Non-destructor releases:
        The \a isFreeUpRemainingItemsWhenDestructorCalled does not effect the normal release mechanizm. If an item is released with \a releaseItem() it will free up it's memory if it's reference counter reaches 0. It's intelligent enough to decide whether the related item is a QObject based item or not and will reparent it before delete, so usually this won't affect other parts based on Qt's parenting memory management.
    */
    explicit ReferenceCountingIdBasedCollection(bool isFreeUpRemainingItemsWhenDestructorCalled, QObject *parent = 0);
    ~ReferenceCountingIdBasedCollection();


    // ------------------------------
    // --- main interface methods ---

    /*! Adds the given \a item to the collection

        @note About the item's ID:
        The given \a item 's ID will be used internally, don't change it after you added it to the collection!
    */
    void addItem(IdBasedItemInterface *item);

    /*! Gets the item related to the given \a itemId

        @param isIncrementItsReferenceCount : if true ( \b default ) then it will increment the item's internal reference count automatically.

        @return The related item, or NULL if not found.
    */
    IdBasedItemInterface *getItem(QString itemId, bool isRetainIt = true);

public Q_SLOTS:
    /*! Retains the related item

        The item's reference-count will be incremented. Usually you don't have to call this directly, you can retain it when you call the \a getItem() method.
    */
    void retainItem(QString itemId);

    /*! Releases the related item

        The item's reference-count will be decremented, and if it reaches 0 it will be deleted as well.
    */
    void releaseItem(QString itemId);


    // ------------------------
    // --- internal methods ---
private:
    void _retainItemWithoutGuard(QString itemId);
    //
    // won't remove from the collection, but will free up the item
    void _safeDeleteItem(ReferenceCountedItem *relatedRefCountedItem);

private:
    bool _isFreeUpRemainingItemsWhenDestructorCalled;
    QMap<QString, ReferenceCountedItem *> _items;
    QMutex _refCountGuardMutex;
};

#endif // REFERENCECOUNTINGIDBASEDCOLLECTION_H
