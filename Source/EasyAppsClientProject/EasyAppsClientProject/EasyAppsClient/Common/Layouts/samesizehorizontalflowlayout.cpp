#include "samesizehorizontalflowlayout.h"

#include <QLayoutItem>
#include <QWidget>
#include <QStyle>


// debug and utility
//#include "math.h"


SameSizeHorizontalFlowLayout::SameSizeHorizontalFlowLayout(QSize itemSize, QWidget *parent) :
        SameSizeLayoutBase(itemSize, parent),
    _isCenterizeHorizontally(false)
{
}

void SameSizeHorizontalFlowLayout::setCenterizeHorizontally(bool isCenterizeHorizontally) {
    this->_isCenterizeHorizontally = isCenterizeHorizontally;
}

//
// multi line specific methods

int SameSizeHorizontalFlowLayout::getItemsPerRow() const {
    return this->getItemsPerRowByMaximumWidth(-1);
}

int SameSizeHorizontalFlowLayout::getItemsPerRowByMaximumWidth(int width) const {

    QMargins marginsToUse = this->contentsMargins();
    width -= (marginsToUse.left() + marginsToUse.right());

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


QRect SameSizeHorizontalFlowLayout::getItemGeometryByIndex(int indexOfItem, QMargins margins) const
{
    if(indexOfItem < 0) {
        WLogS << " ! indexOfItem is < 0, cannot use it";
        return QRect(0, 0, 0, 0);
    }

    QMargins marginsToUse = margins;
//    if(margins.isNull()) {
//        marginsToUse = this->contentsMargins();
//    }

    double calcItemsPerRow = (double)this->getItemsPerRow();
    if(calcItemsPerRow > this->count()) {
        calcItemsPerRow = this->count();
    }

    int rowIndex = floor((double)indexOfItem / (double)this->getItemsPerRow());
    int columnIndex = indexOfItem % this->getItemsPerRow();

    QSize itemSizeHint = this->getItemSize();

    int geomX = 0;
    int geomY = 0;
    if(this->_isCenterizeHorizontally) {
        float itemPartitionsWidthPerRow = ((float)this->getCachedLastReceivedRect().width() - marginsToUse.left() - marginsToUse.right()) / calcItemsPerRow;

        float itemPartitionPosX = ((float)(columnIndex + 1)) * itemPartitionsWidthPerRow;
        float halfItemSize = floor( ((float)itemSizeHint.width()) * 0.5f );
        geomX = marginsToUse.left() + itemPartitionPosX  - floor(itemPartitionsWidthPerRow * 0.5f) - halfItemSize;
        geomY = marginsToUse.top() + rowIndex * itemSizeHint.height();
    }
    else {
        geomX = marginsToUse.left() + columnIndex * itemSizeHint.width();
        geomY = marginsToUse.top() + rowIndex * itemSizeHint.height();
    }

    QRect ret(geomX,  // left
              geomY,     // top
              itemSizeHint.width(),                                      // width
              itemSizeHint.height()                                      // height
              );
//    int x = marginsToUse.left();
//    int y = marginsToUse.top() + rowIndex * itemSizeHint.height();

//    qDebug() << " getItemGeometrynByIndex: " << ret;

    return ret;
}

void SameSizeHorizontalFlowLayout::_performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate)
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" iii performLayoutUpdate, rect: ") << rect << " isOnlyCalculate? " << isOnlyCalculate;
#endif


//    QRect effectiveRect = calculateEffectiveRect(rect);
//    float maxY = effectiveRect.y();

    QMargins marginsToUse = this->contentsMargins();

    //
    // do the layout

//    QSize itemSizeHint = this->getItemSize();

    // get the current sorted list of widgets in the layout
    QList<QLayoutItem *> allWidgetList = this->_itemList;
    int widgetCnt = allWidgetList.size();
    for(int i = 0; i < widgetCnt; i++)
    {
        QLayoutItem *currItem = allWidgetList[i];
        QWidget *currWidget = currItem->widget();

//        QPoint thePos = this->getItemGeometryByIndexUseLastGivenRectAndCalculatedMargins(i);
        QRect itemGeom = this->getItemGeometryByIndex(i, marginsToUse);

        if(!isOnlyCalculate) {
//            currWidget->setGeometry(QRect(QPoint(x, y), itemSizeHint));
            currWidget->setGeometry(itemGeom);
        }

//        maxY = qMax(maxY, (float)itemGeom.y());

//        x = nextX;
    }

}

void SameSizeHorizontalFlowLayout::recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width) {

#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog("recalculate size with width: ") << width;
#endif

    QMargins marginsToUse = this->contentsMargins();
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog(" related Widget margins: ") << this->parentWidget()->contentsMargins();
#endif
    int itemCntInLayout = this->count();
    if(itemCntInLayout >= 1) {

//        int itemsPerRow = 4;
        int itemsPerRow = this->getItemsPerRowByMaximumWidth(width/* - marginsToUse.left() - marginsToUse.right()*/);
        /*if( lastGivenRect.width() > 0 && itemList[0]->minimumSize().width() > 0) {
            itemsPerRow = lastGivenRect.width() / itemList[0]->minimumSize().width();
        }*/
        int columnCnt = ceil((float)itemCntInLayout/ (float)itemsPerRow);

        QSize itemSizeHint = this->getItemSize();

        int minimalWidth = marginsToUse.left() + (itemSizeHint.width() * itemsPerRow) + marginsToUse.right();
        int minimalHeight = marginsToUse.top() + (itemSizeHint.height() * columnCnt) + marginsToUse.bottom();

#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        DLog("recalculateMinimalAndOptimalSizesByTotalLayoutWidth: ") << itemsPerRow << width << minimalWidth << minimalHeight;
#endif

        this->setCalculatedMinimalAndOptimalSizes(QSize(minimalWidth, minimalHeight), QSize(width, minimalHeight));

#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        DLogS << " -- Calculated optimal size: " << this->getCalculatedOptimalSize();
#endif
    }
    else
    {
        this->setCalculatedMinimalAndOptimalSizes(QSize(0, 0), QSize(0, 0));
    }
}


Qt::Orientations SameSizeHorizontalFlowLayout::expandingDirections() const
{
//    return 0;
    return Qt::Vertical | Qt::Horizontal;
}

bool SameSizeHorizontalFlowLayout::hasHeightForWidth() const
{
    return false;
}

int SameSizeHorizontalFlowLayout::heightForWidth(int width) const
{
//    int height = performLayoutUpdate(QRect(0, 0, width, 0), true).height();

//    qDebug() << "heightForWidth: " << height;

    LOG_AS_NOT_IMPLEMENTED;

    int height = 1000;

    return height;
}

//SameSizeHorizontalLayoutingInformations DynamicSameSizeHorizontalFlowLayout::getLayoutingInformations()
//{
//    return SameSizeHorizontalLayoutingInformations(
//            this->getItemSize(),
//            0,
//            this->getItemBaselinePositionY(),
//            0,
//            this->getItemBaselinePositionX(),
//            this->getItemsPerRow());
//}
