#include "dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.h"

#include <QPropertyAnimation>
#include <QKeyEvent>
#include <QEvent>
#include <QScrollArea>
#include <QScrollBar>

// layout and delegates
#include "Common/Layouts/dynamicsamesizehorizontalboxlayout.h"
#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"
#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
#include "Common/mousegrabscrolldelegate.h"

//
#include "Common/IdBasedWidgetCollection/idbasedwidgetcollection.h"
#include "Common/Widgets/idbasedselectableandhighlightablewidgetbase.h"

// configuration
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"
#include "Settings/settings.h"

// utility
#include <QDebug>

DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea(QSize itemSize, SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate, bool isCenterizeItemsHorizontally, QWidget *parent) :
    QScrollArea(parent)
{    
#if 1
    //
    // delegates
    // create, initialize and connect the delegates
    this->_currentWorkingSnapshotItems = new IdBasedWidgetCollection(this);
    this->mouseGrabScrollDelegate = new MouseGrabScrollDelegate(400, 2.0f, 5, 0.3f, this);
    // layout delegates
    WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate = new WidgetListSnapshotHandlerDelegate;
//    iconPickerLayoutSnapshotChangedDelegate->setIconPresenterListDelegate(this->iconPresenterListDelegate);

    //
    // make connections to the layout-delegates

    // make the connections between the sort-result delegate and this window handler/delegate
    connect(snapshotHandlerDelegate, SIGNAL(itemSelected(QWidget *)), this, SLOT(itemSelected(QWidget *)));
    connect(snapshotHandlerDelegate, SIGNAL(itemDeselected(QWidget *)), this, SLOT(itemDeselected(QWidget *)));

    connect(layoutSnapshotChangeDelegate, SIGNAL(layoutChangeStarted(QSize,QSize)), this, SLOT(_layoutSnapshotChangeStarted(QSize,QSize)));
    connect(layoutSnapshotChangeDelegate, SIGNAL(layoutChangeFinished()), this, SLOT(_updateContentSizeByLastGivenWidth()));
#endif

    //
    // create the content layout and widget

    // layout
    contentLayout = new DynamicSameSizeHorizontalBoxLayout(snapshotHandlerDelegate, layoutSnapshotChangeDelegate, itemSize);
    contentLayout->setCenterizeHorizontally(isCenterizeItemsHorizontally);

//    contentLayout->setSizeConstraint(QLayout::SetMinAndMaxSize); // ! don't do this!! this is evil!!

    // widget
    contentWidget = new QWidget(this);
    contentWidget->setLayout(contentLayout);
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

#if 1
    // content margins
    contentWidget->setContentsMargins(0, 0, 0, 0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    this->setContentsMargins(0, 0, 0, 0);

//    contentLayout->setDynamicContentMargins(50, 0, 50, 0);
#endif

#if 1
    // size policy
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#endif


    // init scroll-area
    this->setWidgetResizable(false); // it will resize itself, don't let the scroll area to resize the content - the content will resize the scroll area
    this->setFrameShape(QFrame::NoFrame);
    // connect the mouse-grab-scroll delegate
    mouseGrabScrollDelegate->setTargetScrollArea(this);


    // parenting, layouting
    this->setWidget(contentWidget);

#if 1
    //
    // event filters
    this->installEventFilter(this);
#endif

    //
    // Setup stylesheet
    this->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("ScrollArea/Dynamic/Default/Transparent/style"));
}

//QSize DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::sizeHint() const
//{
//#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
//    DLog(" -- Size Hint: ") << this->contentLayout->minimumSize();
//#endif
////    return this->minimumSize();
//    return this->contentLayout->getCachedCalculatedOptimalSize();
//}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::setContentMarginsForDynamicLayout(int left, int top, int right, int bottom)
{
    this->contentLayout->setDynamicContentMargins(left, top, right, bottom);
}


void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::resizeEvent(QResizeEvent *e) {
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLogS << " -!- Resize event in smart-scroll-area: " << e->size();
#endif

//    _lastGivenContentWidth = event->size().width();
    _lastGivenContentSize = e->size();

#if 0
    // recalculate the optimal size for the content of the scroll area
    this->contentLayout->recalculateMinimalSizeByTotalLayoutWidth(event->size().width());
    // then resize the content layout as well
//    QSize sizeWithNewWidthAndMinimalHeight(event->size().width(), this->contentLayout->getCalculatedOptimalSize().height());
//    this->contentWidget->resize(sizeWithNewWidthAndMinimalHeight);
//    this->contentWidget->resize(event->size());

    QSize calculatedOptimalSize = this->contentLayout->getCalculatedOptimalSize();
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLogS << " - Calculated optimal size: " << calculatedOptimalSize;
#endif
    this->contentWidget->resize(calculatedOptimalSize); // resize the content
    this->contentWidget->adjustSize();
#else
    this->_updateContentSizeByWidthToFit(_lastGivenContentSize.width());
#endif

//    this->contentLayout->update();
//    this->contentWidget->updateGeometry();
    this->updateGeometry();

    QScrollArea::resizeEvent(e);
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::_layoutSnapshotChangeStarted(QSize currentOptimalSize, QSize predictedOptimalSizeAfterLayoutChange)
{
    DLog("_layoutSnapshotChangeStarted: ") << currentOptimalSize << predictedOptimalSizeAfterLayoutChange << _lastGivenContentSize;
    FLAG_FOR_REVIEW_WITH_HINT("Somehow currently the currentOptimalSize is always the same size as predictedOptimalSizeAfterLayoutChange. Last geom will be used instead.");

//    if(predictedOptimalSizeAfterLayoutChange.width() > _lastGivenContentSize.width() || predictedOptimalSizeAfterLayoutChange.height() > _lastGivenContentSize.height())
//    {
//        // the new (predicted) size will be bigger than the current one -> resize to fit the predicted size
//        this->resizeContentWidgetAndUpdateIt(predictedOptimalSizeAfterLayoutChange);
//    }
//    else {
//        // the predicted size is the same or smaller - simply update the content widget and layout
//        this->resizeContentWidgetAndUpdateIt(this->_lastGivenContentSize);
//    }

    // this implementation would be the correct one, if currentOptimalSize would be correct
    if(predictedOptimalSizeAfterLayoutChange.width() > currentOptimalSize.width() || predictedOptimalSizeAfterLayoutChange.height() > currentOptimalSize.height())
    {
        // the new (predicted) size will be bigger than the current one -> resize to fit the predicted size
        this->resizeContentWidgetAndUpdateIt(predictedOptimalSizeAfterLayoutChange);
    }
    else {
        // the predicted size is the same or smaller - simply update the content widget and layout
//        this->_updateContentSizeByLastGivenWidth();
        this->resizeContentWidgetAndUpdateIt(currentOptimalSize);
    }
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::_updateContentSizeByLastGivenWidth()
{
    this->_updateContentSizeByWidthToFit(_lastGivenContentSize.width());
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::_updateContentSizeByWidthToFit(int targetContentWidth)
{
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLogS << "_updateContentSize() - with width: " << targetContentWidth;
#endif

    // recalculate the optimal size for the content of the scroll area
    this->contentLayout->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(targetContentWidth);
    // then resize the content layout as well
//    QSize sizeWithNewWidthAndMinimalHeight(event->size().width(), this->contentLayout->getCalculatedOptimalSize().height());
//    this->contentWidget->resize(sizeWithNewWidthAndMinimalHeight);
//    this->contentWidget->resize(event->size());

    QSize calculatedOptimalSize = this->contentLayout->getCalculatedOptimalSize();
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLogS << " - Calculated optimal size: " << calculatedOptimalSize;
#endif

    this->resizeContentWidgetAndUpdateIt(calculatedOptimalSize);

//    this->updateGeometry();
//    this->update();
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::resizeContentWidgetAndUpdateIt(QSize newSize)
{
    this->contentWidget->resize(newSize); // resize the content
    this->contentWidget->adjustSize();
    this->contentWidget->updateGeometry();
}

// Q_SLOT
void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::_commitSnapshotWithThisOrderAndSetAsCurrentWorking(WidgetListSnapshot sortingResultSnapshot) {

    if(contentLayout->getSnapshotHandlerDelegate() != NULL)
    {
//        this->_idBasedWidgetCollection->_refreshListOrderByIdList(sortingResultSnapshot.getOrderedItems());
        FLAG_FOR_REVIEW_WITH_HINT("This should check whether the items are already added, or do it somehow else to don't require this condition!");
        _currentWorkingSnapshotItems->_setItemsFromWidgetList(sortingResultSnapshot.getOrderedItems());
        contentLayout->getSnapshotHandlerDelegate()->commitSnapshot(sortingResultSnapshot);
    }
    else {
        WLog("Snapshot handler delegate not found!");
    }
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::addIdBasedWidgetToLayout(IdBasedSelectableAndHighlightableWidgetBase *widget, bool isInitializedAndAddedPreviously)
{
    if(widget == NULL) {
        DLog("Invalid input (widget) parameter (NULL). Cannot add it to the list.");
        return;
    }

    if(!isInitializedAndAddedPreviously)
    {
//    IconPresenterWidget *theNewIconPresenter = _createAndAddIconToGui(iconModel);
        widget->installEventFilter(this);

        connect(widget, SIGNAL(mouseEntered(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(mouseEnteredToItem(IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(widget, SIGNAL(mouseLeaved(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(mouseLeavedFromItem(IdBasedSelectableAndHighlightableWidgetBase*)));

        connect(widget, SIGNAL(activated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(itemActivated(IdBasedSelectableAndHighlightableWidgetBase*)));


        // add it to the layout -> needed because of reparenting. Without this the widget will appear in a separate window.
        contentLayout->addWidget(widget);
        widget->hide();
    }
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::addIdBasedWidgetToCurrentWorkingSnapshot(IdBasedSelectableAndHighlightableWidgetBase *widgetToAdd, bool isInitializedAndAddedPreviously)
{
    DLog("addIdBasedWidgetToCurrentWorkingSnapshot");
    if(widgetToAdd == NULL) {
        DLog("Invalid input (widget) parameter (NULL). Cannot add it to the list.");
        return;
    }

    this->addIdBasedWidgetToLayout(widgetToAdd, isInitializedAndAddedPreviously);

    // add it to the all-item list
    this->_currentWorkingSnapshotItems->add(widgetToAdd);


//    contentLayout->getSnapshotHandlerDelegate()->addItem(theNewIconPresenter->getIconModel().getIconTitle());


#if 0
    if(isPresentItAsWell)
    {
//        contentLayout->getSnapshotHandlerDelegate()->addItem(widgetToAdd);

        // convert the id-based list to simple, widget list
        QList<QWidget *> widgets;
        QList<IdBasedWidgetBase *> idBasedWidgets = this->_idBasedWidgetCollection->getAll();
        int widgetCnt = idBasedWidgets.size();
        for(int i = 0; i < widgetCnt; i++) {
            widgets << idBasedWidgets[i];
        }
        // then commit the list
        contentLayout->getSnapshotHandlerDelegate()->commitSnapshot(WidgetListSnapshot(widgets));
    }
#endif
}

// Q_SLOT
void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::itemActivated(IdBasedSelectableAndHighlightableWidgetBase *source)
{
    // delegate the signal
    Q_EMIT itemActivatedSignal(source);
}


// Q_SLOT
void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::itemSelected(QWidget *item)
{
    IdBasedSelectableAndHighlightableWidgetBase *castedItem = dynamic_cast<IdBasedSelectableAndHighlightableWidgetBase *>(item);

    if(castedItem != NULL) {
        castedItem->startHighlighted();
        // delegate the signal
        Q_EMIT itemSelectedSignal(castedItem);
    } else {
        WLog(" ! cannot cast the snapshot-widget");
    }
}

// Q_SLOT
void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::itemDeselected(QWidget *item)
{
    IdBasedSelectableAndHighlightableWidgetBase *castedItem = dynamic_cast<IdBasedSelectableAndHighlightableWidgetBase *>(item);

    if(castedItem != NULL) {
        castedItem->startNotHighlighted();
        // delegate the signal
        Q_EMIT itemDeselectedSignal(castedItem);
    } else {
        WLog(" ! cannot cast the snapshot-widget");
    }
}


// Q_SLOT
void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::mouseEnteredToItem(IdBasedSelectableAndHighlightableWidgetBase *source)
{
    contentLayout->getSnapshotHandlerDelegate()->selectThisItem(source);
}

// Q_SLOT
void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::mouseLeavedFromItem(IdBasedSelectableAndHighlightableWidgetBase *source)
{
    contentLayout->getSnapshotHandlerDelegate()->deselectThisItem(source);
}

// ===========================
// event handling and mouse/keyboard handling / processing

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processKeyPressEventFilter(QKeyEvent *keyEvent)
{
    if( keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Left) {
        contentLayout->getSnapshotHandlerDelegate()->selectPreviousItem();
        ensureCurrentSelectedItemCenterizedInScrollArea();

        return true;
    }
    else if( keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Right) {
        contentLayout->getSnapshotHandlerDelegate()->selectNextItem();
        ensureCurrentSelectedItemCenterizedInScrollArea();

        return true;
    }

    return false;
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processMouseWheelEventFilter(QWheelEvent *wheelEvent)
{
    if (wheelEvent->orientation() == Qt::Horizontal) {
        return false;
    }

#if defined(Q_OS_WIN)
    // emulate the wheel-accelerate-decelerate on Windows (on Mac it's handled by default)
//    this->mouseGrabScrollDelegate->doSmoothDropScroll(0, -1 * wheelEvent->delta() * 2.3f);
    this->mouseGrabScrollDelegate->accelerateWheelForce(0, wheelEvent->delta());

    return true;
#elif defined(Q_OS_MAC)
    return false;
#endif
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processKeyPressEvent(QKeyEvent *keyEvent)
{
    return false;
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processKeyReleaseEvent(QKeyEvent *keyEvent)
{
    return false;
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processMouseButtonPressEvent(QObject *obj, QMouseEvent *e)
{
    return this->mouseGrabScrollDelegate->handleMouseButtonPressEvent(e);
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processMouseMoveEvent(QObject *obj, QMouseEvent *e)
{
    return mouseGrabScrollDelegate->handleMouseMoveEvent(e);
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::processMouseButtonReleaseEvent(QObject *obj, QMouseEvent *e)
{
    return this->mouseGrabScrollDelegate->handleMouseButtonReleaseEvent(e);
}


bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::event(QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        if( this->processKeyPressEvent(keyEvent) )
        {
            event->accept();
            return true;
        }
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        if( this->processKeyReleaseEvent(keyEvent) )
        {
            event->accept();
            return true;
        }
    }

    return QScrollArea::event(event);
}

bool DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if(me)
        {
            if( processMouseButtonPressEvent(obj, me) ) {
                me->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }
    else if(event->type() == QEvent::MouseMove)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if(me)
        {
            if( processMouseMoveEvent(obj, me) ) {
                me->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }
    else if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if(me)
        {
            bool resultOfMouseButtonReleseDelegation = processMouseButtonReleaseEvent(obj, me);
            DLog("resultOfMouseButtonReleseDelegation: ") << resultOfMouseButtonReleseDelegation;
            if( resultOfMouseButtonReleseDelegation ) {
                me->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }

    else if(event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent *>(event);
        if(wheelEvent) {
            if(this->processMouseWheelEventFilter(wheelEvent)) {
                wheelEvent->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }

    else if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        return this->processKeyPressEventFilter(keyEvent);
    }


    return QScrollArea::eventFilter(obj, event);
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::activateCurrentItem()
{
    IdBasedSelectableAndHighlightableWidgetBase *currentCastedItem = this->getCurrentSelectedItemOrTheFirst();

    if(currentCastedItem == NULL) {
        WLog(" ! Some critical error happened, cannot get a valid icon-presenter-widget to activate it.");
        return;
    }

    currentCastedItem->startActivate();
}

IdBasedSelectableAndHighlightableWidgetBase *DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getCurrentSelectedItemOrTheFirst() const
{
    IdBasedSelectableAndHighlightableWidgetBase *currentCastedItem = dynamic_cast<IdBasedSelectableAndHighlightableWidgetBase *>(contentLayout->getSnapshotHandlerDelegate()->getCurrentItem());

    if(currentCastedItem == NULL) {
        // there's no selected item -> try the first one
        DLogS << "Cannot get the current item, so try the first available item.";
        currentCastedItem = dynamic_cast<IdBasedSelectableAndHighlightableWidgetBase *>(contentLayout->getSnapshotHandlerDelegate()->getFirstItem());
    }

//    if(currentCastedItem == NULL) {
//        WLog(" ! Some critical error happened, cannot get a valid icon-presenter-widget to activate it.");
//        return;
//    }

    return currentCastedItem;
}

IdBasedSelectableAndHighlightableWidgetBase *DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getItemByIdFromCurrentWorkingSnapshotOrNull(QString itemId) const
{
    IdBasedSelectableAndHighlightableWidgetBase *currentCastedItem = dynamic_cast<IdBasedSelectableAndHighlightableWidgetBase *>(this->getItemFromCurrentWorkingSnapshotByItemId(itemId));

    return currentCastedItem;
}

IdBasedSelectableAndHighlightableWidgetBase *DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getItemByIdFromWorkingSnapshotOrTheFirstInCommitedSnapshot(QString itemId) const
{
    IdBasedSelectableAndHighlightableWidgetBase *currentCastedItem = this->getItemByIdFromCurrentWorkingSnapshotOrNull(itemId);

    if(currentCastedItem == NULL) {
        // there's no item with this id -> try the first one
        DLogS << "Cannot get the item, so try the first available item. Given item-id: " << itemId;
        currentCastedItem = dynamic_cast<IdBasedSelectableAndHighlightableWidgetBase *>(contentLayout->getSnapshotHandlerDelegate()->getFirstItem());
    }

//    if(currentCastedItem == NULL) {
//        WLog(" ! Some critical error happened, cannot get a valid icon-presenter-widget to activate it.");
//        return;
//    }

    return currentCastedItem;
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::ensureItemCenterizedInScrollArea(QString itemId)
{
    IdBasedSelectableAndHighlightableWidgetBase *currentCastedItem = this->getItemByIdFromCurrentWorkingSnapshotOrNull(itemId);

    if(currentCastedItem == NULL) {
        DLog(" Cannot get the given item.");
        return;
    }

    if(currentCastedItem != NULL) {
        QPropertyAnimation *scrollAnim = new QPropertyAnimation(
                this->verticalScrollBar(),
                "value");
        scrollAnim->setDuration(300);
        scrollAnim->setStartValue(this->verticalScrollBar()->value());
        scrollAnim->setEndValue(currentCastedItem->geometry().y() - this->height() / 2 + currentCastedItem->geometry().height() / 2);
        scrollAnim->setEasingCurve(QEasingCurve::InOutQuad); // InOutQuad, OutExpo
        scrollAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::ensureCurrentSelectedItemCenterizedInScrollArea()
{
    IdBasedSelectableAndHighlightableWidgetBase *currentCastedItem = this->getCurrentSelectedItemOrTheFirst();

    if(currentCastedItem == NULL) {
        WLog(" Cannot get a valid icon-presenter-widget to activate it.");
        return;
    }

    if(currentCastedItem != NULL) {
        QPropertyAnimation *scrollAnim = new QPropertyAnimation(
                this->verticalScrollBar(),
                "value");
        scrollAnim->setDuration(300);
        scrollAnim->setStartValue(this->verticalScrollBar()->value());
        scrollAnim->setEndValue(currentCastedItem->geometry().y() - this->height() / 2 + currentCastedItem->geometry().height() / 2);
        scrollAnim->setEasingCurve(QEasingCurve::InOutQuad); // InOutQuad, OutExpo
        scrollAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

QList<QWidget *> DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getCurrentWorkingSnapshotAsWidgetList() const
{
    return this->_currentWorkingSnapshotItems->_getAllAsWidgetList();
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::clearCurrentWorkingSnapshot()
{
    this->_currentWorkingSnapshotItems->_removeAll(false);
}

QList<IdBasedWidgetBase *> DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getCurrentWorkingSnapshot() const
{
    return this->_currentWorkingSnapshotItems->getAll();
}

IdBasedWidgetBase *DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getItemFromCurrentWorkingSnapshotByItemId(QString itemId) const
{
    return this->_currentWorkingSnapshotItems->getFirstById(itemId);
}

void DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::_commitCurrentWorkingSnapshot() {
    contentLayout->getSnapshotHandlerDelegate()->commitSnapshot(WidgetListSnapshot(this->_currentWorkingSnapshotItems->_getAllAsWidgetList()));
}

IdBasedWidgetCollection *DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::__getIdBasedWidgetCollection() const {
    FLAG_AS_DEPRECATED;
    return this->_currentWorkingSnapshotItems;
}

WidgetListSnapshotHandlerDelegate *DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea::getSnapshotHandlerDelegate() const {
    return this->contentLayout->getSnapshotHandlerDelegate();
}
