#ifndef PAGETRANSITIONMANAGERDELEGATEINTERFACE_H
#define PAGETRANSITIONMANAGERDELEGATEINTERFACE_H

#include <QObject>
#include <QBoxLayout>

#include "Common/Page/pagewidgetbase.h"

//
// Manages the transition between 2 pages
// -> it has to
class PageTransitionManagerDelegateInterface : public QObject
{
    Q_OBJECT

public:
    PageTransitionManagerDelegateInterface(QObject *parent=0) : QObject(parent) {}
    virtual ~PageTransitionManagerDelegateInterface() {}

    //
    // ! this will change the containerLayout
    // ! and it will remove _from_ from the containerLayout
    // ! from (page) have to be in the input layout
    //      and to (page) have to be NOT in the input layout
    virtual void performTransition(PageWidgetBase *sourcePage, PageWidgetBase *targetPage, QBoxLayout *containerLayout, bool isShouldDeleteSourcePageAfterTransition = false) = 0;
    //
    // this have to stop the transition, if any in progress.
    //  can happen if the transition-manager-delegate is changed to another -> in this case the first one should stop
    //  and then the new one can be used
    virtual void jumpToFinalState(/*bool isCallPageDidAppearOnTargetPage = false*/) = 0;

Q_SIGNALS:
    void transitionFinished(PageWidgetBase *sourcePage, PageWidgetBase *targetPat, bool isShouldDeleteSourcePage);
};

#endif // PAGETRANSITIONMANAGERDELEGATEINTERFACE_H
