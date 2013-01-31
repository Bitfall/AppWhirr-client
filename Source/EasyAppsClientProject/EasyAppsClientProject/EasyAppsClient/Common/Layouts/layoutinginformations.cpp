#include "layoutinginformations.h"

SameSizeHorizontalLayoutingInformations::SameSizeHorizontalLayoutingInformations(QSize itemSize,
                                                                                 int horizontalSpacingBetweenItems, int itemBaselinePositionY,
                                                                                 int verticalSpacingBetweenItems, int itemBaselinePositionX,
                                                                                 int itemsPerRowCnt):
        itemSize(itemSize),
        horizontalSpacing(horizontalSpacingBetweenItems),
        itemBaselinePositionY(itemBaselinePositionY),
        verticalSpacingBetweenItems(verticalSpacingBetweenItems),
        itemBaselinePositionX(itemBaselinePositionX),
        itemsPerRowCnt(itemsPerRowCnt)
{
}

SameSizeHorizontalLayoutingInformations SameSizeHorizontalLayoutingInformations::CreateEmpty()
{
    return SameSizeHorizontalLayoutingInformations(QSize(0, 0), 0, 0, 0, 0, 1);
}

QSize SameSizeHorizontalLayoutingInformations::getItemSize() const
{
    return this->itemSize;
}

int SameSizeHorizontalLayoutingInformations::getHorizontalSpacingBetweenItems() const
{
    return this->horizontalSpacing;
}

int SameSizeHorizontalLayoutingInformations::getItemBaselinePositionY() const
{
    return this->itemBaselinePositionY;
}

int SameSizeHorizontalLayoutingInformations::getVerticalSpacingBetweenItems() const {
    return this->verticalSpacingBetweenItems;
}

int SameSizeHorizontalLayoutingInformations::getItemBaselinePositionX() const
{
    return this->itemBaselinePositionX;
}

int SameSizeHorizontalLayoutingInformations::getItemsPerRowCount() const {
    return this->itemsPerRowCnt;
}
