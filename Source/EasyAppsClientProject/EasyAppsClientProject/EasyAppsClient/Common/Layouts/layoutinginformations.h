#ifndef LAYOUTINGINFORMATIONS_H
#define LAYOUTINGINFORMATIONS_H

#include <QSize>

//
// Layout informations about a same-sized, horizontal layout
class SameSizeHorizontalLayoutingInformations
{
public:
    SameSizeHorizontalLayoutingInformations(QSize itemSize,
                                            int horizontalSpacingBetweenItems, int itemBaselinePositionY,
                                            int verticalSpacingBetweenItems, int itemBaselinePositionX,
                                            int itemsPerRowCnt);

    // factory method to create an empty layout-informations object
    static SameSizeHorizontalLayoutingInformations CreateEmpty();


    QSize getItemSize() const;

    int getHorizontalSpacingBetweenItems() const;
    int getItemBaselinePositionY() const;

    int getVerticalSpacingBetweenItems() const;
    int getItemBaselinePositionX() const;

    int getItemsPerRowCount() const;


private:
    QSize itemSize;

    int horizontalSpacing;
    int itemBaselinePositionY;

    int verticalSpacingBetweenItems;
    int itemBaselinePositionX;

    int itemsPerRowCnt;
};

#endif // LAYOUTINGINFORMATIONS_H
