#include "samesizelayoutbase.h"

#include <QWidget>

SameSizeLayoutBase::SameSizeLayoutBase(QSize itemSize, QWidget *parent) :
    QLayout(parent),
    _itemSize(itemSize)
{

}

SameSizeLayoutBase::~SameSizeLayoutBase()
{
#if 0 // the snapshot-delegate's responsibility to delete the items
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
#endif
}

// ===============================================================
// item management

int SameSizeLayoutBase::count() const
{
    // QList::size() returns the number of QLayoutItems in the list
//    return itemList.size();
    return this->_itemList.size();
}

QLayoutItem *SameSizeLayoutBase::itemAt(int index) const
{
    // QList::value() performs index checking, and returns 0 if we are
    // outside the valid range
//    return itemList.value(index);
#ifdef ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
//    LOG_AS_NOT_IMPLEMENTED_WITH_HINT(" !!! NOT IMPLEMENTED (DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::itemAt)");
#endif
    return NULL;
}

QLayoutItem *SameSizeLayoutBase::takeAt(int index)
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

void SameSizeLayoutBase::addWidget(QWidget *w) {
    // currently it does absolutely nothing - in the future it has to be implemented
    //  and merged with the snapshot-delegate add

    // only reparenting
    addChildWidget(w);
    // and call addItem, to recalculate chached data
    this->addItem(new QWidgetItem(w));

//    QLayout::addWidget(w);
}

void SameSizeLayoutBase::addItem(QLayoutItem *item)
{
    _itemList.append(item);

    this->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(this->getCachedLastReceivedRect().width());
    this->parentWidget()->resize(this->getCalculatedOptimalSize());
    this->parentWidget()->setMinimumSize(this->getCalculatedMinimalSize());
}

// ===============================================================
// layout infos

QSize SameSizeLayoutBase::sizeHint() const
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" - SizeHint:") << this->getCalculatedOptimalSize();
#endif
//    return minimumSize();
    return this->getCalculatedOptimalSize();
}

QSize SameSizeLayoutBase::minimumSize() const
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" - MinimumSize:") << this->getCalculatedMinimalSize();
#endif

    return this->getCalculatedMinimalSize();
}

//void DynamicSameSizeLayoutBase::_recalculateOptimalSizeByLastReceivedWidth() {
//    this->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(this->_lastReceivedRect.size().width());
//}

// ===============================================================
// layouting

QRect SameSizeLayoutBase::getItemGeometryByIndexAndCalculatedMargins(int indexOfItem) const {
    return this->getItemGeometryByIndex(indexOfItem, QMargins()); // NULL margin will force to calculate / get the margins
}

void SameSizeLayoutBase::setGeometry(const QRect &rect)
{
#if defined(ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING)
    DLogS << " -- Set geom: " << rect;
#endif

//    _lastReceivedRect = rect;

    // ensure optimal size is recalculated


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

    this->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(rect.width());

    // ensure optimal size is recalculated
//    this->recalculateOptimalSizeByWidth(rect.width());

    // ensure optimal size is recalculated -> have to be in the actual implementation of the layout, not necessary here
//    this->recalculateOptimalSizeByWidth(rect.width());

    //    this->parentWidget()->setMinimumSize(optimalSize);

    //lastGivenRect = rect;
}

void SameSizeLayoutBase::performLayoutUpdate(const QRect &rect, bool isOnlyCalculate)
{
    this->_performLayoutUpdateImplementation(rect, isOnlyCalculate);
}

void SameSizeLayoutBase::setCalculatedMinimalAndOptimalSizes(QSize calculatedMinimalSize, QSize calculatedOptimalSize)
{
#if defined(ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING)
    DLog("calculated minimal size: ") << calculatedMinimalSize;
    DLog("calculated optimal size: ") << calculatedOptimalSize;
#endif
    this->_lastCalculatedMinimalSize = calculatedMinimalSize;
    this->_lastCalculatedOptimalSize = calculatedOptimalSize;
}

QSize SameSizeLayoutBase::getCalculatedOptimalSize() const
{
    return this->_lastCalculatedOptimalSize;
}

QSize SameSizeLayoutBase::getCalculatedMinimalSize() const
{
    return this->_lastCalculatedMinimalSize;
}

QRect SameSizeLayoutBase::calculateEffectiveRect(const QRect &rect) const
{
    QMargins margins = this->contentsMargins();
    QRect effectiveRect = rect.adjusted(+margins.left(), +margins.top(), -margins.right(), -margins.bottom());

    return effectiveRect;
}

const QRect SameSizeLayoutBase::getCachedLastReceivedRect() const {
    return this->_lastReceivedRect;
}

//void DynamicSameSizeLayoutBase::snapshotChanged() {
//#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
//    DLogS << " Snapshot changed";
//#endif

//    // invalidate caches and recalculate them
////    this->recalculateBiggestItemSize();

//    // TODO: maybe the optimal-size should be recalculated as well
//    this->_recalculateOptimalSizeByLastReceivedWidth(); // ?

//    this->parentWidget()->updateGeometry();
//}


//void DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::setItemSize(QSize itemSize) {
//    this->itemSize = itemSize;
//}

QSize SameSizeLayoutBase::getItemSize() const {
    if(this->_itemSize.width() < 0 || this->_itemSize.height() < 0) {
        // stretch in that direction
        if(this->_itemSize.width() < 0 && this->_itemSize.height() < 0) {
            // stretch in both directions -> this is bad, very bad (in almost every case)
            return this->_lastReceivedRect.size();
        }
        else if(this->_itemSize.width() < 0) {
            // will be stretched in width
            return QSize(this->_lastReceivedRect.size().width(), this->_itemSize.height());
        }
        else {
            // will be stretched in height
            return QSize(this->_itemSize.width(), this->_lastReceivedRect.size().height());
        }
    }

    return this->_itemSize;
}
