#ifndef DEFAULTPAGETRANSITIONMANAGERDELEGATE_H
#define DEFAULTPAGETRANSITIONMANAGERDELEGATE_H

#include <QWidget>
#include "pagetransitionmanagerdelegateinterface.h"

class CrossFaderWidget;

/*! Implements the \a PageTransitionManagerDelegateInterface with a cross-fade page transition effect

    @note Important!!!
    There's a visual artifact if the pages' main/root layout has content-margins, so we have to force it to NOT have any. The \a performTransition() will set the content-margins for both the sourcePage and for the targetPage to 0 to avoid the artifact. You should design your pages with 0 main/root layout content-margins, but it will be forced if you forget it. You can use content-margins on sub-layouts and widgets and you can set content-margins even for the page-widget, only the page's root/main layout has to have 0 content-margins.
*/
class SimpleFadePageTransitionManagerDelegate : public PageTransitionManagerDelegateInterface
{
    Q_OBJECT
public:
    explicit SimpleFadePageTransitionManagerDelegate(QObject *parent = 0);
    ~SimpleFadePageTransitionManagerDelegate();


    void performTransition(PageWidgetBase *sourcePage, PageWidgetBase *targetPage, QBoxLayout *containerLayout, bool isShouldDeleteSourcePageAfterTransition);

    void jumpToFinalState(/*bool isCallPageDidAppearOnTargetPage = false*/);

private Q_SLOTS:
    void _transitionFinishedSlot(/*bool isCallPageDidAppearOnTargetPage = true*/);

private:
    void insertPageToLayout(int indexWhereToPutTheTargetWidget, PageWidgetBase *thePage);

private:
    PageWidgetBase *_tmpSourcePage;
    PageWidgetBase *_tmpTargetPage; // this one has to exist when transitionFinished is performed
    bool _isShouldDeleteSourcePageAfterTransition;
    QBoxLayout *_tmpCurrentLayout; // holds the given layout between performTransition and transitionFinished

    int indexWhereToPutTheTargetWidget;

    CrossFaderWidget *faderWidget;

    bool isInTransition;
};

#endif // DEFAULTPAGETRANSITIONMANAGERDELEGATE_H
