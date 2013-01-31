#ifndef DYNAMICSAMESIZEHORIZONTALLINELAYOUT_H
#define DYNAMICSAMESIZEHORIZONTALLINELAYOUT_H

#include "dynamicdelegatebasedhighperformancesamesizelayoutbase.h"

//
// This class is a general class. Implement the implementation dependent codes in it's delegates.
//
// LINE layout
//
// ! Same-size means that all the input items will be used as they would be the same size
// ! Line means: all the inserted Widgets will be in a single horizontal line
class DynamicSameSizeHorizontalLineLayout : public DynamicDelegateBasedHighPerformanceSameSizeLayoutBase
{
    Q_OBJECT
public:
    explicit DynamicSameSizeHorizontalLineLayout(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
                                                         SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate,
                                                         QSize itemSize,
                                                         QWidget *parent = 0);

    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;

    SameSizeHorizontalLayoutingInformations getLayoutingInformations();

    QRect getItemGeometryByIndex(int indexOfItem, QMargins margins) const;

    //
    // force recalculate methods
    void recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width);

protected:
    void _performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate = false);


private:
    int getItemsPerRow();
};

#endif // DYNAMICSAMESIZEHORIZONTALLINELAYOUT_H
