#include "dynamicsamesizehorizontalboxlayout.h"

#include <QLayoutItem>
#include <QWidget>
#include <QStyle>

#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
#include "Common/Layouts/layoutinginformations.h"

// debug and utility
#include "math.h"
#include <QDebug>


DynamicSameSizeHorizontalBoxLayout::DynamicSameSizeHorizontalBoxLayout(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
                                                                                       SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate,
                                                                                       QSize itemSize,
                                                                                       QWidget *parent) :
        DynamicDelegateBasedHighPerformanceSameSizeLayoutBase(snapshotHandlerDelegate, layoutSnapshotChangeDelegate, itemSize, parent),
    _isCenterizeHorizontally(false)
{
}

void DynamicSameSizeHorizontalBoxLayout::setCenterizeHorizontally(bool isCenterizeHorizontally) {
    this->_isCenterizeHorizontally = isCenterizeHorizontally;
}

//
// multi line specific methods

int DynamicSameSizeHorizontalBoxLayout::getItemsPerRow() const {
    return this->getItemsPerRowByMaximumWidth(-1);
}

int DynamicSameSizeHorizontalBoxLayout::getItemsPerRowByMaximumWidth(int width) const {

    QMargins marginsToUse = this->getDynamicContentMargins();

    int theTargetWidth = width;
    if(width <= 0) {
        theTargetWidth = this->getCachedLastReceivedRect().size().width() - marginsToUse.left() - marginsToUse.right();
    }

    if(theTargetWidth <= 0) {
        // if still <= 0 then return constant 1
        return 1;
    }

    int biggestWidth = this->getItemSize().width();
    if(biggestWidth <= 0) {
        // probably there's no item in the layout
        return 1;
    }

    int itemsPerRow = floor((float)theTargetWidth / (float)biggestWidth);
    if(itemsPerRow > 1) {
        return itemsPerRow;
    }

    return 1;
}


QRect DynamicSameSizeHorizontalBoxLayout::getItemGeometryByIndex(int indexOfItem, QMargins margins) const
{
    if(indexOfItem < 0) {
        qWarning() << " ! indexOfItem is < 0, cannot use it";
        return QRect(0, 0, 0, 0);
    }

    QMargins marginsToUse = margins;
    if(margins.isNull()) {
        marginsToUse = this->getDynamicContentMargins();
    }
//    int allItemCount = this->getSnapshotHandlerDelegate()->getAllItemCount();

    double calcItemsPerRow = (double)this->getItemsPerRow();
    if(calcItemsPerRow > this->getSnapshotHandlerDelegate()->getAllItemCount()) {
        calcItemsPerRow = this->getSnapshotHandlerDelegate()->getAllItemCount();
    }

    int rowIndex = floor((double)indexOfItem / (double)this->getItemsPerRow());
    int columnIndex = indexOfItem % this->getItemsPerRow();

    int geomX = 0;
    int geomY = 0;
    if(this->_isCenterizeHorizontally) {
        float itemPartitionsWidthPerRow = ((float)this->getCachedLastReceivedRect().width() - marginsToUse.left() - marginsToUse.right()) / calcItemsPerRow;

        float itemPartitionPosX = ((float)(columnIndex + 1)) * itemPartitionsWidthPerRow;
        float halfItemSize = floor( ((float)this->getItemSize().width()) * 0.5f );
        geomX = marginsToUse.left() + itemPartitionPosX  - floor(itemPartitionsWidthPerRow * 0.5f) - halfItemSize;
        geomY = marginsToUse.top() + rowIndex * this->getItemSize().height();
    }
    else {
        geomX = marginsToUse.left() + columnIndex * this->getItemSize().width();
        geomY = marginsToUse.top() + rowIndex * this->getItemSize().height();
    }

    QRect ret(geomX,  // left
              geomY,     // top
              this->getItemSize().width(),                                      // width
              this->getItemSize().height()                                      // height
              );
//    int x = marginsToUse.left();
//    int y = marginsToUse.top() + rowIndex * this->getItemSize().height();

//    qDebug() << " getItemGeometrynByIndex: " << ret;

    return ret;
}

void DynamicSameSizeHorizontalBoxLayout::_performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate)
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" iii performLayoutUpdate, rect: ") << rect << " isOnlyCalculate? " << isOnlyCalculate;
#endif


    QRect effectiveRect = calculateEffectiveRect(rect);
//    float x = effectiveRect.x();
//    float y = effectiveRect.y();
    float maxY = effectiveRect.y();

    QMargins marginsToUse = this->getDynamicContentMargins();

    //
    // do the layout

    QSize itemSizeHint = this->getItemSize();

    // get the current sorted list of widgets in the layout
    QList<QWidget *> allWidgetList = this->getSnapshotHandlerDelegate()->getAllItems();
    int widgetCnt = allWidgetList.size();
    for(int i = 0; i < widgetCnt; i++)
    {
        QWidget *currWidget = allWidgetList[i];

//        QPoint thePos = this->getItemGeometryByIndexUseLastGivenRectAndCalculatedMargins(i);
        QRect itemGeom = this->getItemGeometryByIndex(i, marginsToUse);

        if(!isOnlyCalculate) {
//            currWidget->setGeometry(QRect(QPoint(x, y), itemSizeHint));
            currWidget->setGeometry(itemGeom);
        }

        maxY = qMax(maxY, (float)itemGeom.y());

//        x = nextX;
    }

    QSize calculatedUsedSize = QSize(effectiveRect.width(),
                                     maxY + itemSizeHint.height() + marginsToUse.bottom());
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    qDebug() << "Calculated used size in performLayout: " << calculatedUsedSize;
#endif

#if 0
    // return the optimal size
    return calculatedUsedSize;
#endif
}

void DynamicSameSizeHorizontalBoxLayout::recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width) {

#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog("recalculate size with width: ") << width;
#endif

    QMargins marginsToUse = this->getDynamicContentMargins();
    int itemCntInLayout = this->count();
    if(itemCntInLayout >= 1) {

//        int itemsPerRow = 4;
        int itemsPerRow = this->getItemsPerRowByMaximumWidth(width);
        /*if( lastGivenRect.width() > 0 && itemList[0]->minimumSize().width() > 0) {
            itemsPerRow = lastGivenRect.width() / itemList[0]->minimumSize().width();
        }*/
        int columnCnt = ceil((float)itemCntInLayout/ (float)itemsPerRow);

        QSize itemSizeHint = this->getItemSize();

        int minimalWidth = marginsToUse.left() + itemSizeHint.width() * itemsPerRow + marginsToUse.right();
        int minimalHeight = marginsToUse.top() + itemSizeHint.height() * columnCnt + marginsToUse.bottom();

        this->setCalculatedMinimalAndOptimalSizes(QSize(minimalWidth, minimalHeight), QSize(width, minimalHeight));

#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        qDebug() << " -- Calculated optimal size: " << this->getCalculatedOptimalSize();
#endif
    }
    else
    {
        this->setCalculatedMinimalAndOptimalSizes(QSize(0, 0), QSize(0, 0));
    }
}


Qt::Orientations DynamicSameSizeHorizontalBoxLayout::expandingDirections() const
{
//    return 0;
    return Qt::Vertical | Qt::Horizontal;
}

bool DynamicSameSizeHorizontalBoxLayout::hasHeightForWidth() const
{
    return false;
}

int DynamicSameSizeHorizontalBoxLayout::heightForWidth(int width) const
{
//    int height = performLayoutUpdate(QRect(0, 0, width, 0), true).height();

//    qDebug() << "heightForWidth: " << height;

    LOG_AS_NOT_IMPLEMENTED;

    int height = 1000;

    return height;
}

SameSizeHorizontalLayoutingInformations DynamicSameSizeHorizontalBoxLayout::getLayoutingInformations()
{
    return SameSizeHorizontalLayoutingInformations(
            this->getItemSize(),
            0,
            this->getItemBaselinePositionY(),
            0,
            this->getItemBaselinePositionX(),
            this->getItemsPerRow());
}
