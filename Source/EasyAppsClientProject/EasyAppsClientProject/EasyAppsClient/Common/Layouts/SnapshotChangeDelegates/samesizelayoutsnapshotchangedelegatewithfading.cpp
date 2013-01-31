#include "samesizelayoutsnapshotchangedelegatewithfading.h"

//#include "Common/FadingItemBase/fadingitembase.h"
#include "Common/fadingiteminterface.h"

#include <QPropertyAnimation>

SameSizeLayoutSnapshotChangeDelegateWithFading::SameSizeLayoutSnapshotChangeDelegateWithFading(QObject *parent) :
    SameSizeLayoutSnapshotChangeDelegateBase(parent)
{
}

void SameSizeLayoutSnapshotChangeDelegateWithFading::addHideAnimationToCurrentAnimationGroup(QWidget *widgetToHide)
{
//    FadingItemBase *castedWidget = dynamic_cast<FadingItemBase *>(widgetToHide);
//    if(castedWidget == NULL) {
//        WLog("Cannot cast the widget to the desired type.");
//        return;
//    }

//    QPropertyAnimation *fadeAnim = new QPropertyAnimation(widgetToHide, "fadeValue");
//    fadeAnim->setDuration(this->getShowHideAnimationDuration());
//    fadeAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
//    fadeAnim->setStartValue(1.0f);
//    fadeAnim->setEndValue(0.0f);

//    this->_addAnimationToGroup(fadeAnim);

    FadingItemInterface *castedWidget = dynamic_cast<FadingItemInterface *>(widgetToHide);
    if(castedWidget == NULL) {
        WLog("Cannot cast the widget to the desired type.");
        return;
    }

    castedWidget->startFadeOutAnimation();
}

void SameSizeLayoutSnapshotChangeDelegateWithFading::addShowAnimationToCurrentAnimationGroup(QWidget *widgetToShow,
                                                                                 SameSizeHorizontalLayoutingInformations layoutingInformations)
{
//    FadingItemBase *castedWidget = dynamic_cast<FadingItemBase *>(widgetToShow);
//    if(castedWidget == NULL) {
//        WLog("Cannot cast the widget to the desired type.");
//        return;
//    }

//    QPropertyAnimation *fadeAnim = new QPropertyAnimation(widgetToShow, "fadeValue");
//    fadeAnim->setDuration(this->getShowHideAnimationDuration());
//    fadeAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
//    fadeAnim->setStartValue(0.0f);
//    fadeAnim->setEndValue(1.0f);

//    this->_addAnimationToGroup(fadeAnim);

    FadingItemInterface *castedWidget = dynamic_cast<FadingItemInterface *>(widgetToShow);
    if(castedWidget == NULL) {
        WLog("Cannot cast the widget to the desired type.");
        return;
    }

    castedWidget->startFadeInAnimation();
}
