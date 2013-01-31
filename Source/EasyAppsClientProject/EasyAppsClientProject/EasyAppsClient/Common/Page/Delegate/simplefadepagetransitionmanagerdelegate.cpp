#include "simplefadepagetransitionmanagerdelegate.h"

#include "Common/Widgets/crossfaderwidget.h"

#include <QTimer>

SimpleFadePageTransitionManagerDelegate::SimpleFadePageTransitionManagerDelegate(QObject *parent) :
    PageTransitionManagerDelegateInterface(parent),
    isInTransition(false),
    _isShouldDeleteSourcePageAfterTransition(false)
{
    indexWhereToPutTheTargetWidget = -1;

    faderWidget = new CrossFaderWidget;
//    faderWidget->setStyleSheet(QString("CrossFaderWidget { background-color: #ff0000; }"));
    connect(faderWidget, SIGNAL(fadeFinish()), this, SLOT(_transitionFinishedSlot()));
}

SimpleFadePageTransitionManagerDelegate::~SimpleFadePageTransitionManagerDelegate()
{
    DLog("destruct");
    this->jumpToFinalState();

    faderWidget->setParent(0);
    delete faderWidget;
}

void SimpleFadePageTransitionManagerDelegate::performTransition(PageWidgetBase *sourcePage, PageWidgetBase *targetPage, QBoxLayout *containerLayout, bool isShouldDeleteSourcePageAfterTransition)
{
#ifdef ENABLE_PAGE_TRANSITION_LOGGING
    DLog("perform page transition");
#endif

    // finalize the current one, if any
    this->jumpToFinalState(/*false*/);

    //
    // ! workaround: there's a visual artifact if the pages' main/root layout has content-margins, so we have to force it to NOT have any
    QLayout *tmpPageMainLayout = sourcePage->layout();
    if(tmpPageMainLayout != NULL) {
        tmpPageMainLayout->setContentsMargins(0, 0, 0, 0);
    }
    tmpPageMainLayout = targetPage->layout();
    if(tmpPageMainLayout != NULL) {
        tmpPageMainLayout->setContentsMargins(0, 0, 0, 0);
    }



    QSize requiredPageSize = sourcePage->geometry().size();

//    QSize requiredPageSize = QSize(sourcePage->geometry().size().width() + sourcePage->contentsMargins().left() + sourcePage->contentsMargins().right(), sourcePage->geometry().size().height());

    // !!!
    // something is not right with Qt when you want to determine the geometry of a not-yet-shown

    _tmpCurrentLayout = containerLayout;
    indexWhereToPutTheTargetWidget = _tmpCurrentLayout->indexOf(sourcePage);
    if(indexWhereToPutTheTargetWidget == -1)
    {
        WLog("The source/from page is not found - cannot performTransiton");
        return;
    }

    // indicate the transition process
    isInTransition = true;


//    QRect fromGeom = from->geometry();
    QPoint sourcePagePosition = sourcePage->geometry().topLeft();

#if 0
    QLayout *sourcePageLayout = sourcePage->layout();
    if(sourcePageLayout != NULL) {
        QMargins sourcePageLayoutMargins = sourcePageLayout->contentsMargins();
        sourcePagePosition = QPoint(sourcePagePosition.x() - sourcePageLayoutMargins.left()/* - sourcePageLayoutMargins.right()*/, sourcePagePosition.y() - sourcePageLayoutMargins.top()/* - sourcePageLayoutMargins.bottom()*/);
    }
#endif

//    QPoint sourcePagePosition(0, 0);
    QRect sourcePageGeom = QRect(sourcePagePosition, sourcePage->geometry().size());

#if 0
    QLayout *sourcePageLayout = sourcePage->layout();
    if(sourcePageLayout != NULL) {
        QMargins sourcePageLayoutMargins = sourcePageLayout->contentsMargins();
        if(sourcePageLayoutMargins.left() != 0 && sourcePageLayoutMargins.right() != 0 && sourcePageLayoutMargins.top() != 0 && sourcePageLayoutMargins.bottom() != 0)
        {
            sourcePageGeom = QRect(QPoint(sourcePagePosition.x() - (sourcePageLayoutMargins.left() / 2), sourcePagePosition.y() - (sourcePageLayoutMargins.top() / 2)), QSize(requiredPageSize.width() + (sourcePageLayoutMargins.right() / 2), requiredPageSize.height() + (sourcePageLayoutMargins.bottom() / 2)));
        }
    }
#endif

//    QRect targetPageGeom = QRect(targetPagePosition, targetPage->geometry().size());
//    QRect targetPageGeom = QRect(0, 0, requiredPageSize.width(), requiredPageSize.height());
    QRect targetPageGeom = sourcePageGeom;

    // remove the from page
    _tmpCurrentLayout->removeWidget(sourcePage);
    sourcePage->setParent(NULL);
    sourcePage->hide(); // hide it to prevent it to surprisely appear

    // add the to page and force a layout - to get it's target geometry
//    to->setAttribute(Qt::WA_WState_ExplicitShowHide, true);
////    to->setAttribute(Qt::WA_WState_Visible, true);
//    to->setAttribute(Qt::WA_WState_Hidden, false);
//    insertPageToLayout(indexWhereToPutTheTargetWidget, targetPage);

    // -- TEST
//    _currentTmpLayout->update();
//    _currentTmpLayout->invalidate();
//    _currentTmpLayout->activate();
//    targetPage->show();
    targetPage->setGeometry(QRect(0, 0, requiredPageSize.width(), requiredPageSize.height()));
//    targetPage->updateGeometry();
//    targetPage->update();
//    targetPage->adjustSize();
#if 0
    _tmpCurrentLayout->update();
    _tmpCurrentLayout->invalidate();
    _tmpCurrentLayout->activate();
#endif
//    QRect toGeom = to->geometry();
//    QPoint targetPagePosition = targetPage->geometry().topLeft();
//    to->setAttribute(Qt::WA_WState_ExplicitShowHide, false);
//    to->setAttribute(Qt::WA_WState_Hidden, true);
    // --


    _tmpSourcePage = sourcePage;
    _tmpTargetPage = targetPage;
    _isShouldDeleteSourcePageAfterTransition = isShouldDeleteSourcePageAfterTransition;


#ifdef ENABLE_PAGE_TRANSITION_LOGGING
//    DLogS << "max page size: " << maxSizeOfPages;
    DLogS << "from geom: " << sourcePageGeom;
    DLogS << "to geom: " << targetPageGeom;
    DLog("Required page-size: ") << requiredPageSize,
#endif


    // resize the fader to source's size
    faderWidget->setFixedSize(requiredPageSize);
    // insert it to the place of source
    _tmpCurrentLayout->insertWidget(indexWhereToPutTheTargetWidget, faderWidget);
    faderWidget->show();
    _tmpCurrentLayout->activate();



    // and start the fadeing-magic
#ifdef ENABLE_PAGE_TRANSITION_LOGGING
    DLog("Take snapshots of widgets");
#endif
    faderWidget->takeSnapshotsOfWidgetsAndFadeBetweenThem(sourcePage, targetPage, sourcePageGeom, targetPageGeom);
#ifdef ENABLE_PAGE_TRANSITION_LOGGING
    DLog(" - snapshot done");
#endif

    DLog("Fade transition started.");

//    QTimer::singleShot(0, this, SLOT(_transitionFinishedSlot())); // for testing 'without fade animation page transition'
}

void SimpleFadePageTransitionManagerDelegate::jumpToFinalState(/*bool isCallPageDidAppearOnTargetPage*/)
{
#ifdef ENABLE_PAGE_TRANSITION_LOGGING
    DLog("jump to final state");
#endif
    if(isInTransition) {
        // stop fading
        faderWidget->stopFading();

        // and actualize - if we were in animation!
//        _transitionFinishedSlot(isCallPageDidAppearOnTargetPage);
    }
}

void SimpleFadePageTransitionManagerDelegate::_transitionFinishedSlot()
{
//#ifdef ENABLE_PAGE_TRANSITION_LOGGING
    DLog(" !! transition finished ");
//#endif

    // remove the fader-widget
    _tmpCurrentLayout->removeWidget(faderWidget);
    faderWidget->setParent(NULL);
    faderWidget->hide();

    this->insertPageToLayout(indexWhereToPutTheTargetWidget, _tmpTargetPage);

//    if(isCallPageDidAppearOnTargetPage) {
        _tmpTargetPage->pageDidAppear();
//    }

    Q_EMIT transitionFinished(_tmpSourcePage, _tmpTargetPage, _isShouldDeleteSourcePageAfterTransition);

    isInTransition = false;

    DLog(" !! transition finished - not in transition ");
}

void SimpleFadePageTransitionManagerDelegate::insertPageToLayout(int indexToInsert, PageWidgetBase *thePage)
{
    // and replace it with the real target widget
    _tmpCurrentLayout->insertWidget(indexToInsert, thePage/*, 0, Qt::AlignTop | Qt::AlignCenter*/);
    thePage->show();
}
