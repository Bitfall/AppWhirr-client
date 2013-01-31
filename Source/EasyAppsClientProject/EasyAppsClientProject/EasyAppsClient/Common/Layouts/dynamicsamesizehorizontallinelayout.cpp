#include "dynamicsamesizehorizontallinelayout.h"

#include <QLayoutItem>
#include <QWidget>
#include <QStyle>

#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
#include "Common/Layouts/layoutinginformations.h"

// debug and utility
#include <QDebug>

DynamicSameSizeHorizontalLineLayout::DynamicSameSizeHorizontalLineLayout(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
                                                                                         SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate,
                                                                                         QSize itemSize,
                                                                                         QWidget *parent) :
        DynamicDelegateBasedHighPerformanceSameSizeLayoutBase(snapshotHandlerDelegate, layoutSnapshotChangeDelegate, itemSize, parent)
{
}

void DynamicSameSizeHorizontalLineLayout::recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width)
{
    QMargins marginsToUse = this->getDynamicContentMargins();

    int desiredWidth = marginsToUse.left() + this->getSnapshotHandlerDelegate()->getAllItemCount() * this->getItemSize().width() + marginsToUse.right();
    int desiredHeight = marginsToUse.top() + this->getItemSize().height() + marginsToUse.bottom();

    this->setCalculatedMinimalAndOptimalSizes(QSize(desiredWidth, desiredHeight), QSize(width, desiredHeight));
}

int DynamicSameSizeHorizontalLineLayout::getItemsPerRow() {
    return this->count();
}

SameSizeHorizontalLayoutingInformations DynamicSameSizeHorizontalLineLayout::getLayoutingInformations()
{
    return SameSizeHorizontalLayoutingInformations(
            this->getItemSize(),
            0,
            this->getItemBaselinePositionY(),
            0,
            this->getItemBaselinePositionX(),
            this->getItemsPerRow());
}

QRect DynamicSameSizeHorizontalLineLayout::getItemGeometryByIndex(int indexOfItem, QMargins margins) const
{
    if(indexOfItem < 0) {
        qWarning() << " ! indexOfItem is < 0, cannot use it";
        return QRect(0, 0, 0, 0);
    }

    QMargins marginsToUse = margins;
    if(margins.isNull()) {
        marginsToUse = this->getDynamicContentMargins();
    }

    int columnIndex = indexOfItem;

    QRect ret(marginsToUse.left() + columnIndex * this->getItemSize().width(),  // left
              marginsToUse.top(),                                               // top
              this->getItemSize().width(),                                      // width
              this->getItemSize().height()                                      // height
              );

    return ret;
}


void DynamicSameSizeHorizontalLineLayout::_performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate)
{
    float maxX = getItemBaselinePositionX();

    QMargins marginsToUse = this->getDynamicContentMargins();


    //
    // do the layout

//    QSize itemSizeHint = getItemSize();

    // get the current sorted list of widgets in the layout
    QList<QWidget *> allWidgetList = this->getSnapshotHandlerDelegate()->getAllItems();
    int widgetCnt = allWidgetList.size();
    for(int i = 0; i < widgetCnt; i++)
    {
        QWidget *currWidget = allWidgetList[i];

        QRect itemGeom = this->getItemGeometryByIndex(i, marginsToUse);

        if(!isOnlyCalculate) {
            //          item->setGeometry(QRect(QPoint(x, itemY), itemSizeHint));
            //            currWidget->setGeometry(QRect(QPoint(x, y), biggestItemSizeHint));
            currWidget->setGeometry(itemGeom);
        }

        //        float nextY = y + biggestItemSizeHint.height();

        //        y = nextY;
        maxX = qMax(maxX, (float)itemGeom.x());
    }

#if 0
    // return the optimal size
    return QSize(maxX + itemSizeHint.width() + marginsToUse.right(), // maxX contains the left margin
                 marginsToUse.top() + itemSizeHint.height() + marginsToUse.bottom());
#endif
}


Qt::Orientations DynamicSameSizeHorizontalLineLayout::expandingDirections() const
{
//    return 0;
    return Qt::Horizontal;
}

bool DynamicSameSizeHorizontalLineLayout::hasHeightForWidth() const
{
    return false;
}

int DynamicSameSizeHorizontalLineLayout::heightForWidth(int width) const
{
//    int height = performLayoutUpdate(QRect(0, 0, width, 0), true).height();

//    qDebug() << "heightForWidth: " << height;

    int height = 0;

    LOG_AS_NOT_IMPLEMENTED;

    return height;
}
