#include "dynamicdelegatebasedhighperformancesamesizelayoutbase.h"

#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"
#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
#include "Common/Layouts/layoutinginformations.h"

// debug and utility
#include <QDebug>

DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::DynamicDelegateBasedHighPerformanceSameSizeLayoutBase(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
                                                                                                             SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate,
                                                                                                             QSize itemSize,
                                                                                                             QWidget *parent) :
    QLayout(parent),
    itemSize(itemSize),
    isLayoutUpdateBlockedFlag(false)
{
    this->snapshotHandlerDelegate = snapshotHandlerDelegate;
    this->snapshotHandlerDelegate->setParent(this);

    this->layoutSnapshotChangeDelegate = layoutSnapshotChangeDelegate;
    this->layoutSnapshotChangeDelegate->setParent(this);

    // connect the snapshot-handler and the snapshot-change
    connect(this->snapshotHandlerDelegate, SIGNAL(itemsChanged(WidgetListSnapshot, WidgetListSnapshot)),
            this->layoutSnapshotChangeDelegate, SLOT(changeSnapshot(WidgetListSnapshot,WidgetListSnapshot)));

    connect(this->snapshotHandlerDelegate, SIGNAL(snapshotChanged()),
            this, SLOT(snapshotChanged()));
}

DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::~DynamicDelegateBasedHighPerformanceSameSizeLayoutBase()
{
#if 0 // the snapshot-delegate's responsibility to delete the items
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
#endif
}

// ===============================================================
// delegate

WidgetListSnapshotHandlerDelegate *DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getSnapshotHandlerDelegate() const
{
    return this->snapshotHandlerDelegate;
}

SameSizeLayoutSnapshotChangeDelegateBase *DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getSnapshotChangeDelegate() const
{
    return this->layoutSnapshotChangeDelegate;
}

QList<QWidget *> DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getCurrentLayoutDataSnapshot() const
{
    if(this->snapshotHandlerDelegate == NULL)
    {
        qDebug("snapshot handler delegate is NULL. set it first.");
        return QList<QWidget *>();
    }

    return this->snapshotHandlerDelegate->getAllItems();
}

// ===============================================================
// item management

int DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::count() const
{
    // QList::size() returns the number of QLayoutItems in the list
//    return itemList.size();
    return this->snapshotHandlerDelegate->getAllItemCount();
}

QLayoutItem *DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::itemAt(int index) const
{
    // QList::value() performs index checking, and returns 0 if we are
    // outside the valid range
//    return itemList.value(index);
#ifdef ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
//    LOG_AS_NOT_IMPLEMENTED_WITH_HINT(" !!! NOT IMPLEMENTED (DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::itemAt)");
#endif
    return NULL;
}

QLayoutItem *DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::takeAt(int index)
{
    // DON'T DELETE / REMOVE items directly from the layout - do it through snapshot-change
    //      and when the change finishes the item is free to remove / delete
    //  -> but this should be centerized here, in the layout !!!
#ifdef ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT(" !!! NOT IMPLEMENTED (DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::takeAt)");
#endif

#if 0
    if (index >= 0 && index < itemList.size())
    {
        QLayoutItem *itemToRemove = itemList.takeAt(index);
        //contentTotalMinimumSize.setHeight(this->getMinimumContentHeight());

        recalculateBiggestItemSize();

        return itemToRemove;
    }
#endif

    return NULL;
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::addWidget(QWidget *w) {
    // currently it does absolutely nothing - in the future it has to be implemented
    //  and merged with the snapshot-delegate add

    // only reparenting
    addChildWidget(w);
    // and call addItem, to recalculate chached data
    this->addItem(new QWidgetItem(w));

//    QLayout::addWidget(w);
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::addItem(QLayoutItem *item)
{
    // add to the item-list
//    itemList.append(item);
    // !!!

    //contentTotalMinimumSize.setWidth(contentTotalMinimumSize.width() + item->minimumSize().width());
    //qDebug(QString("curr min w: %1").arg(contentTotalMinimumSize.width()));

#ifdef ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT(" !!! NOT (truely) IMPLEMENTED (DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::addItem)");
#endif

//    recalculateBiggestItemSize();
//    this->recalculateOptimalSizeByLastReceivedWidth();
}

// ===============================================================
// layout infos

QSize DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::sizeHint() const
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" - SizeHint:") << this->getCalculatedOptimalSize();
#endif
//    return minimumSize();
    return this->getCalculatedOptimalSize();
}

QSize DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::minimumSize() const
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" - MinimumSize:") << this->getCalculatedMinimalSize();
#endif

    return this->getCalculatedMinimalSize();
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::_recalculateOptimalSizeByLastReceivedWidth() {
    this->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(this->_lastReceivedRect.size().width());
}

// ===============================================================
// layouting

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setLayoutUpdateBlocked(bool isBlocked, bool isUpdateLayoutAfter)
{
#if 0
    if(isUpdateLayout)
    {
        performLayoutUpdate(this->geometry());
    }
#endif

    isLayoutUpdateBlockedFlag = isBlocked;

    if(isUpdateLayoutAfter)
    {
//        performLayoutUpdate(this->geometry());
        this->performLayoutUpdate(this->_lastReceivedRect);
    }
}


QRect DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getItemGeometryByIndexAndCalculatedMargins(int indexOfItem) const {
    return this->getItemGeometryByIndex(indexOfItem, QMargins()); // NULL margin will force to calculate / get the margins
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setGeometry(const QRect &rect)
{
#if defined(ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING)
    DLogS << " -- Set geom: " << rect;
#endif

//    _lastReceivedRect = rect;

    // ensure optimal size is recalculated
    this->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(rect.width());

    QLayout::setGeometry(rect);
    //    QLayout::setGeometry(QRect(rect.x(), rect.y(), 1000, 1000));

    _lastReceivedRect = rect;

//    // cache the size
//    if(rect.size() != this->_lastReceivedRect.size()) {
//        // update the last received size
//        _lastReceivedRect = rect;
//    }

    // perform layout update
    performLayoutUpdate(rect);

    // ensure optimal size is recalculated
//    this->recalculateOptimalSizeByWidth(rect.width());

    // ensure optimal size is recalculated -> have to be in the actual implementation of the layout, not necessary here
//    this->recalculateOptimalSizeByWidth(rect.width());

    //    this->parentWidget()->setMinimumSize(optimalSize);

    //lastGivenRect = rect;
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::performLayoutUpdate(const QRect &rect, bool isOnlyCalculate)
{
    if(this->getIsLayoutUpdateBlocked()/* && !isOnlyCalculate*/)
    {
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        DLog("Layout update is BLOCKED");
#endif
        if(this->isShouldBlockLayoutUpdateDuringAnimation())
        {
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        DLog("But the layout allows to resize during animation.");
#endif
//        return rect.size();
        return;
        }
    }
    else {
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        DLog("Layout update is NOT blocked.");
#endif
    }

    this->_performLayoutUpdateImplementation(rect, isOnlyCalculate);
}

int DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getItemBaselinePositionY() const
{
    return 0;
}

int DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getItemBaselinePositionX() const
{
    return 0;
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setCalculatedMinimalAndOptimalSizes(QSize calculatedMinimalSize, QSize calculatedOptimalSize)
{
    this->_lastCalculatedMinimalSize = calculatedMinimalSize;
    this->_lastCalculatedOptimalSize = calculatedOptimalSize;
}

QSize DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getCalculatedOptimalSize() const
{
    return this->_lastCalculatedOptimalSize;
}

QSize DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getCalculatedMinimalSize() const
{
    return this->_lastCalculatedMinimalSize;
}

QMargins DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getDynamicContentMargins() const {
#if 0 // for some reason it doesn't work correctly
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    return QMargins(left, top, right, bottom);
#endif

    return this->_dynamicContentMargins;
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setDynamicContentMargins(int left, int top, int right, int bottom) {
    this->setDynamicContentMargins(QMargins(left, top, right, bottom));
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setDynamicContentMargins(QMargins margins) {
    this->_dynamicContentMargins = margins;
}

QRect DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::calculateEffectiveRect(const QRect &rect) const
{
    QMargins margins = this->getDynamicContentMargins();
    QRect effectiveRect = rect.adjusted(+margins.left(), +margins.top(), -margins.right(), -margins.bottom());

    return effectiveRect;
}

bool DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getIsLayoutUpdateBlocked() const {
    return this->isLayoutUpdateBlockedFlag;
}

const QRect DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getCachedLastReceivedRect() const {
    return this->_lastReceivedRect;
}

void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::snapshotChanged() {
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLogS << " Snapshot changed";
#endif

    // invalidate caches and recalculate them
//    this->recalculateBiggestItemSize();

    // TODO: maybe the optimal-size should be recalculated as well
    this->_recalculateOptimalSizeByLastReceivedWidth(); // ?

    this->parentWidget()->updateGeometry();
}

#if 0
void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setItemSize(QSize itemSize) {
    this->itemSize = itemSize;
}
#endif

QSize DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getItemSize() const {
    if(this->itemSize.width() < 0 || this->itemSize.height() < 0) {
        // stretch in that direction
        if(this->itemSize.width() < 0 && this->itemSize.height() < 0) {
            // stretch in both directions -> this is bad, very bad (in almost every case)
            return this->_lastReceivedRect.size();
        }
        else if(this->itemSize.width() < 0) {
            // will be stretched in width
            return QSize(this->_lastReceivedRect.size().width(), this->itemSize.height());
        }
        else {
            // will be stretched in height
            return QSize(this->itemSize.width(), this->_lastReceivedRect.size().height());
        }
    }

    return this->itemSize;
}
