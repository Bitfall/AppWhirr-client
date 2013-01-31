#ifndef DYNAMICSAMESIZEVERTICALLINELAYOUT_H
#define DYNAMICSAMESIZEVERTICALLINELAYOUT_H

#include "dynamicdelegatebasedhighperformancesamesizelayoutbase.h"

class DynamicSameSizeVerticalLineLayout : public DynamicDelegateBasedHighPerformanceSameSizeLayoutBase
{
    Q_OBJECT
public:
    explicit DynamicSameSizeVerticalLineLayout(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
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
    QSize getItemSize() const;

//    bool isShouldBlockLayoutUpdateDuringAnimation() { return false; }


private:
    int getItemsPerRow() const;

};

#endif // DYNAMICSAMESIZEVERTICALLINELAYOUT_H
