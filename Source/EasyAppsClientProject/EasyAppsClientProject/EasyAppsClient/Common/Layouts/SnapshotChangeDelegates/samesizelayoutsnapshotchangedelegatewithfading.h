#ifndef SAMESIZELAYOUTSNAPSHOTCHANGEDELEGATEWITHFADING_H
#define SAMESIZELAYOUTSNAPSHOTCHANGEDELEGATEWITHFADING_H

#include "samesizelayoutsnapshotchangedelegatebase.h"

class SameSizeLayoutSnapshotChangeDelegateWithFading : public SameSizeLayoutSnapshotChangeDelegateBase
{
public:
    explicit SameSizeLayoutSnapshotChangeDelegateWithFading(QObject *parent = 0);

protected:
    virtual void addHideAnimationToCurrentAnimationGroup(QWidget *widgetToHide);
    virtual void addShowAnimationToCurrentAnimationGroup(QWidget *widgetToShow, SameSizeHorizontalLayoutingInformations layoutingInformations);
};

#endif // SAMESIZELAYOUTSNAPSHOTCHANGEDELEGATEWITHFADING_H
