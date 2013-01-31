#include "dynamicsamesizeverticallinelayout.h"


#include <QLayoutItem>
#include <QWidget>
#include <QStyle>

#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
#include "Common/Layouts/layoutinginformations.h"

// debug and utility
#include <QDebug>
#include <math.h>


DynamicSameSizeVerticalLineLayout::DynamicSameSizeVerticalLineLayout(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
                                                                     SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate,
                                                                     QSize itemSize,
                                                                     QWidget *parent) :
    DynamicDelegateBasedHighPerformanceSameSizeLayoutBase(snapshotHandlerDelegate, layoutSnapshotChangeDelegate, itemSize, parent)
{
}


void DynamicSameSizeVerticalLineLayout::recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width)
{
    if(this->getSnapshotHandlerDelegate()->getAllItemCount() > 0) {
        QMargins marginsToUse = this->getDynamicContentMargins();

        int desiredHeight = marginsToUse.top() + this->getSnapshotHandlerDelegate()->getAllItemCount() * this->getItemSize().height() + marginsToUse.bottom();
        this->setCalculatedMinimalAndOptimalSizes(QSize(width, desiredHeight), QSize(width, desiredHeight));
    }
    else {
        this->setCalculatedMinimalAndOptimalSizes(QSize(0, 0), QSize(0, 0));
    }
}

int DynamicSameSizeVerticalLineLayout::getItemsPerRow() const {
    return 1;
}

SameSizeHorizontalLayoutingInformations DynamicSameSizeVerticalLineLayout::getLayoutingInformations()
{
    return SameSizeHorizontalLayoutingInformations(
            this->getItemSize(),
            0,
            this->getItemBaselinePositionY(),
            0,
            this->getItemBaselinePositionX(),
            this->getItemsPerRow());
}

QSize DynamicSameSizeVerticalLineLayout::getItemSize() const {
    QMargins marginsToUse = this->getDynamicContentMargins();

    return QSize(this->getCachedLastReceivedRect().width() - marginsToUse.right() - marginsToUse.left(),
                 DynamicDelegateBasedHighPerformanceSameSizeLayoutBase::getItemSize().height());
}

QRect DynamicSameSizeVerticalLineLayout::getItemGeometryByIndex(int indexOfItem, QMargins margins) const
{
    if(indexOfItem < 0) {
        WLogS << " ! indexOfItem is < 0, cannot use it";
        return QRect(0, 0, 0, 0);
    }

    QMargins marginsToUse = margins;
    if(margins.isNull()) {
        marginsToUse = this->getDynamicContentMargins();
    }
//    int allItemCount = this->getSnapshotHandlerDelegate()->getAllItemCount();

//    int columnIndex = 1;
    int rowIndex = indexOfItem;

    QRect ret(marginsToUse.left(),                                              // left
              marginsToUse.top() + rowIndex * this->getItemSize().height(),     // top
              this->getCachedLastReceivedRect().width() - marginsToUse.right() - marginsToUse.left(),   // width
              this->getItemSize().height()                                      // height
              );
//    int x = marginsToUse.left();
//    int y = marginsToUse.top() + rowIndex * this->getItemSize().height();

//    qDebug() << " getItemGeometryByIndex: " << ret;
//    qDebug() << "margins to use: " << marginsToUse;

//    DLog(" --- Get item geom by index: ") << indexOfItem << ret << marginsToUse;

    return ret;
}

void DynamicSameSizeVerticalLineLayout::_performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate)
{
//    DLog(" --- perform layout update (rect, cached rect): ") << rect << this->getCachedLastReceivedRect();

//    QRect effectiveRect = rect;
//    QSize minSize = minimumSize();
//    effectiveRect.setSize(minSize);
//    float x = effectiveRect.x();
//    float y = getItemBaselinePositionY();
    float maxY = getItemBaselinePositionY();

    QMargins marginsToUse = this->getDynamicContentMargins();
//    qDebug() << " !-! margins: " << marginsToUse;

    //
    // do the layout

//    QSize itemSizeHint = this->getItemSize();

    // get the current sorted list of widgets in the layout
    QList<QWidget *> allWidgetList = this->getSnapshotHandlerDelegate()->getAllItems();
    int widgetCnt = allWidgetList.size();
    for(int i = 0; i < widgetCnt; i++)
    {
        QWidget *currWidget = allWidgetList[i];

//        QPoint thePos = this->getItemGeometryByIndex(i, marginsToUse);
        QRect itemGeom = this->getItemGeometryByIndex(i, marginsToUse);

        if(!isOnlyCalculate) {
//          item->setGeometry(QRect(QPoint(x, itemY), itemSizeHint));
//            currWidget->setGeometry(QRect(QPoint(x, y), biggestItemSizeHint));
//            DLog(" --- set item geom for item: ") << itemGeom << currWidget->objectName() << currWidget->metaObject()->className();
//            DLog(" ---- further info: ") << currWidget->minimumSize() << currWidget->maximumSize();
//            currWidget->setMinimumSize(itemGeom.size());
//            currWidget->setMaximumSize(itemGeom.size());
            currWidget->setFixedSize(itemGeom.size());
            currWidget->setGeometry(itemGeom);
//            currWidget->update();
//            qDebug() << " ,,, curr wig geom: " << itemGeom;
//            qDebug() << "margins to use: " << marginsToUse;
        }

//        float nextY = y + biggestItemSizeHint.height();

//        y = nextY;
        maxY = qMax(maxY, (float)itemGeom.y());
    }

#if 0
    // return the optimal size
    return QSize(rect.width(), // full width
                 maxY + itemSizeHint.height() + marginsToUse.bottom()); // maxY contains the top-margin
#endif
}


Qt::Orientations DynamicSameSizeVerticalLineLayout::expandingDirections() const
{
//    return 0;
    return Qt::Vertical;
}

bool DynamicSameSizeVerticalLineLayout::hasHeightForWidth() const
{
    return false;
}

int DynamicSameSizeVerticalLineLayout::heightForWidth(int width) const
{
//    int height = performLayoutUpdate(QRect(0, 0, width, 0), true).height();

//    qDebug() << "heightForWidth: " << height;

    int height = 1;

    return height;
}
