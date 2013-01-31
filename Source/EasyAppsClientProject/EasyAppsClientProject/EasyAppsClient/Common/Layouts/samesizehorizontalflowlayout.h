#ifndef DYNAMICSAMESIZEHORIZONTALBOXLAYOUT_H
#define DYNAMICSAMESIZEHORIZONTALBOXLAYOUT_H


#include "samesizelayoutbase.h"

//
// This class is a general class. Implement the implementation dependent codes in it's delegates.
//
// Flow layout
//
// ! Same-size means that all the input items will be used as they would be the same size
// ! Horizontal Box means: it fills it's lines horizontally as far as it can, then it starts a new line
// ! Layouts it's items from left to right, HorizontalAlign: Left, VerticalAlign: Top
// ! It only performs layout - won't change it's items size! Only their position.
class SameSizeHorizontalFlowLayout : public SameSizeLayoutBase
{
    Q_OBJECT
public:
    explicit SameSizeHorizontalFlowLayout(QSize _itemSize, QWidget *parent = 0);


    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;


//    void insertNewWidget(QWidget *widget, int indexBefore);

//    SameSizeHorizontalLayoutingInformations getLayoutingInformations();

    QRect getItemGeometryByIndex(int indexOfItem, QMargins margins) const;

    //
    void setCenterizeHorizontally(bool isCenterizeHorizontally);

    //
    void recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width);

//public Q_SLOTS:
    //void changeToThisSnapshot(QList<)

protected:
    void _performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate = false);

private:

    //int getMinimumContentHeight();

    //
    // if the return value would be < 1 then it returns 1 for safety
    int getItemsPerRow() const;
    //
    // if width <= 0 then the last known rect size's width will be used
    int getItemsPerRowByMaximumWidth(int width) const;

private:

    //QSize contentTotalMinimumSize; // have to be recalculated every time when an item's size changes

    bool _isCenterizeHorizontally;




//    QRect lastGivenRect;
};

#endif // DYNAMICSAMESIZEHORIZONTALBOXLAYOUT_H
