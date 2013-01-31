#include "samesizelayoutsnapshotchangedelegatebase.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "Common/Layouts/dynamicdelegatebasedhighperformancesamesizelayoutbase.h"

// configurations
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

// debug and utility
#include "itemmoveddata.h"

SameSizeLayoutSnapshotChangeDelegateBase::SameSizeLayoutSnapshotChangeDelegateBase(QObject *parent) :
        QObject(parent),

        isProcessingSnapshotChangeFlag(false),
        isCompactSnapshotChangeAvailable(false),
        isCompactSnapshotSourceNotYetDefined(true)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    this->showHideAnimationDuration = guiConfManager->getIntForId("SnapshotChange/Default/ShowHideAnimation/duration");
    this->moveAnimationDuration = guiConfManager->getIntForId("SnapshotChange/Default/MoveAnimation/duration");

    // create the animation-group of this layout-change delegate
    {
        currentAnimGroup = new QParallelAnimationGroup(this);
        connect(currentAnimGroup, SIGNAL(finished()), this, SLOT(layoutChangeAnimationsFinished()));
    }
}

void SameSizeLayoutSnapshotChangeDelegateBase::_changeSnapshotImplementation(WidgetListSnapshot sourceSnapshot, WidgetListSnapshot targetSnapshot,
                                                                       SameSizeHorizontalLayoutingInformations layoutingInformations)
{
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
    DLogS << "_changeSnapshotImplementation";
#endif
    DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *castedParentLayout = this->getCastedParentLayout();
    if(castedParentLayout == NULL) {
        WLog(" ! Cannot get or cast Parent layout - cannot perform the snapshot-change");
        return;
    }

    QList<ItemMovedData> movedItems;
    //QString targetSnapshotItem;
    //QString sourceSnapshotItem;
    QList<QWidget *> targetSnapshotOrderedItems = targetSnapshot.getOrderedItems();
    QList<QWidget *> sourceSnapshotOrderedItems = sourceSnapshot.getOrderedItems();
    int targetSnapshotItemCount = targetSnapshotOrderedItems.size();
    int sourceSnapshotItemCount = sourceSnapshotOrderedItems.size();

    //
    // scan for added or moved target-items
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
    DLog(" -- target");
    DLogS << " targetSnapshotOrderedItems cnt: " << targetSnapshotItemCount;
#endif
    for(int i = 0; i < targetSnapshotItemCount; i++)
    {
        bool isFound = false;
        for(int j = 0; j < sourceSnapshotItemCount; j++)
        {
            if(targetSnapshotOrderedItems[i] == sourceSnapshotOrderedItems[j])
            {
                isFound = true;

                // check it's position
                if(i != j)
                {
                    // it's position is changed
                    movedItems << ItemMovedData(targetSnapshotOrderedItems[i], j, i);
                }

                break;
            }
        }

        if(!isFound)
        {
            // it's not in the source, so add it
            QWidget *notYetPresentedWidget = targetSnapshotOrderedItems[i];
            if(notYetPresentedWidget != NULL)
            {
                // to ensure it will be in the right place also add a move
                //movedItems << ItemMovedData(targetSnapshotOrderedItems[i], 0, i);
                int idx = i;

#if 0
                int rowIndex = idx % layoutingInformations.getItemsPerRowCount();
                int columnIndex = 0;
                if(layoutingInformations.getItemsPerRowCount() > 0) {
                    columnIndex = floor((double)idx / (double)layoutingInformations.getItemsPerRowCount());
                }

                //QPoint thePos = QPoint(idx * layoutInformations.getItemSize().width(), layoutInformations.getItemPositionY());
//                QPoint thePos = QPoint(idx * GuiConfigurations::iconImagePresenterWidgetSizeHint().width(), // !warning! hack alert!
//                                       layoutInformations.getItemPositionY());

                QPoint thePos = QPoint(rowIndex * layoutingInformations.getItemSize().width(), columnIndex * layoutingInformations.getItemSize().height());
#endif
                QRect itemGeom = castedParentLayout->getItemGeometryByIndexAndCalculatedMargins(idx);

                notYetPresentedWidget->setGeometry(itemGeom);

                notYetPresentedWidget->show(); // set it visible
                this->addShowAnimationToCurrentAnimationGroup(notYetPresentedWidget, layoutingInformations);

#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
                DLog("SHOW Widget, with index:") << idx;
#endif
            }
            else
            {
                WLog(" !!! icon presenter not found - in changeSnapshot");
            }
        }
    }

    //
    // have to scan for elements which are removed or hidden (presented in source but not in the target)
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
    DLog(" -- source");
    DLogS << " sourceSnapshotItemCount cnt: " << sourceSnapshotItemCount;
#endif
    for(int i = 0; i < sourceSnapshotItemCount; i++)
    {
        bool isFound = false;
        for(int j = 0; j < targetSnapshotItemCount; j++)
        {
            if(sourceSnapshotOrderedItems[i] == targetSnapshotOrderedItems[j])
            {
                isFound = true;
                break;
            }
        }

        if(!isFound)
        {
            // it's not in the target, so hide it
            QWidget *hideThisWidget = sourceSnapshotOrderedItems[i];

            if(hideThisWidget != NULL)
            {
                this->_addToCurrentlyHidingItems(hideThisWidget);
                this->addHideAnimationToCurrentAnimationGroup(hideThisWidget);

#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
                DLog("HIDE Widget, with index:") << i;
#endif
            }
            else
            {
                WLog(" !!! icon presenter not found - in changeSnapshot");
            }
        }
    }

    //
    // make the move animations
    if(movedItems.size() > 0)
    {
        for(int i = 0; i < movedItems.size(); i++)
        {
//            QString item = movedItems[i].getItem();
//            IconPresenterWidget *iconPresenter = this->iconPresenterListDelegate->getIconPresenterWidgetByTitle(item);
            QWidget *moveThisWidget = movedItems[i].getItem();
            if(moveThisWidget != NULL)
            {
                int oldIndex = movedItems[i].getOldIndex();
                int newIndex = movedItems[i].getNewIndex();

#if 0
                int oldRowIndex = oldIndex % layoutingInformations.getItemsPerRowCount();
                int oldColumnIndex = 0;
                if(layoutingInformations.getItemsPerRowCount() > 0) {
                    oldColumnIndex = floor((double)oldIndex / (double)layoutingInformations.getItemsPerRowCount());
                }

                int newRowIndex = newIndex % layoutingInformations.getItemsPerRowCount();
                int newColumnIndex = 0;
                if(layoutingInformations.getItemsPerRowCount() > 0) {
                    newColumnIndex = floor((double)newIndex / (double)layoutingInformations.getItemsPerRowCount());
                }

                //QPoint thePos = QPoint(idx * layoutInformations.getItemSize().width(), layoutInformations.getItemPositionY());
//                QPoint thePos = QPoint(idx * GuiConfigurations::iconImagePresenterWidgetSizeHint().width(), // !warning! hack alert!
//                                       layoutInformations.getItemPositionY());

                QPoint oldPos = QPoint(oldRowIndex * layoutingInformations.getItemSize().width(), oldColumnIndex * layoutingInformations.getItemSize().height());
                QPoint newPos = QPoint(newRowIndex * layoutingInformations.getItemSize().width(), newColumnIndex * layoutingInformations.getItemSize().height());

//                QPoint oldPos = QPoint(oldIndex * layoutInformations.getItemSize().width(), layoutInformations.getItemPositionY());
//                QPoint newPos = QPoint(newIndex * layoutInformations.getItemSize().width(), layoutInformations.getItemPositionY());
#endif

                QRect oldGeom = castedParentLayout->getItemGeometryByIndexAndCalculatedMargins(oldIndex);
                QRect newGeom = castedParentLayout->getItemGeometryByIndexAndCalculatedMargins(newIndex);

#if 0
                QPropertyAnimation *animation = new QPropertyAnimation(iconPresenter, "geometry");
                animation->setDuration(GuiConfigurations::iconPresenterMoveAnimationDuration());
                animation->setStartValue(QRect(oldPos, iconPresenter->geometry().size()));
                animation->setEndValue(QRect(newPos, iconPresenter->geometry().size()));
                animation->setEasingCurve(QEasingCurve::InOutQuart);
                animGroup->addAnimation(animation);
#endif

                //DLog() << " !! MOVE" << oldPos.x() << oldPos.y() << newPos.x() << newPos.y();
                this->addMoveAnimationToCurrentAnimationGroup(moveThisWidget, oldGeom.topLeft(), newGeom.topLeft());

#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
                DLog("MOVE Widget, with from index to index:") << oldIndex << newIndex;
#endif
            }
            else
            {
                WLog(" !!! icon presenter not found - in changeSnapshot, move animations");
            }
        }
    }
}

void SameSizeLayoutSnapshotChangeDelegateBase::changeSnapshot(WidgetListSnapshot sourceSnapshot, WidgetListSnapshot targetSnapshot)
{
    // check whether a change is currently in progress so we should only store (and compress)
    //  this change or we should make it right now (if we are not busy right now)
    if(isProcessingSnapshotChangeFlag)
    {
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog("isProcessingSnapshotChangeFlag = true");
        DLogS << "sourceSnapshot cnt: " << sourceSnapshot.getOrderedItemCount();
        DLogS << "targetSnapshot cnt: " << targetSnapshot.getOrderedItemCount();
#endif
        //snapshotQueue.enqueue(SnapshotQueueItem(sourceSnapshot, targetSnapshot));

        isCompactSnapshotChangeAvailable = true;
        if(isCompactSnapshotSourceNotYetDefined)
        {
            isCompactSnapshotSourceNotYetDefined = false;
            compactSnapshotChange.setSourceSnapshot(sourceSnapshot);
        }

        compactSnapshotChange.setTargetSnapshot(targetSnapshot);

        return;
    }

    DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *castedParent = this->getCastedParentLayout();
    if(castedParent == NULL)
    {
        WLog("Parent is not the type we need.");
        return;
    }

    QSize currentOptimalSize = castedParent->getCalculatedOptimalSize();

    isProcessingSnapshotChangeFlag = true; // block until it finished - queue the requests
    SameSizeHorizontalLayoutingInformations layoutInformations = castedParent->getLayoutingInformations();

    // force-recalculate the sizes before layout-blocking
    // -> the layout has the required informations about the new-state (new item cound, size, ...)
//    castedParent->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(castedParent->getCachedLastReceivedRect().width());
    castedParent->_recalculateOptimalSizeByLastReceivedWidth();

    QSize predictedOptimalSizeAfterLayoutChange = castedParent->getCalculatedOptimalSize();

    Q_EMIT layoutChangeStarted(currentOptimalSize, predictedOptimalSizeAfterLayoutChange);
    castedParent->setLayoutUpdateBlocked(true, false);

    // clear the current animation set - it's allowed here, we are not in a change-progress (animation)
    this->clearCurrentAnimationGroup();

    //
    // calculate and create the change-animations
    this->_changeSnapshotImplementation(sourceSnapshot, targetSnapshot, layoutInformations);

    //
    // and start the change-animations
    this->startCurrentAnimationGroup();
}

void SameSizeLayoutSnapshotChangeDelegateBase::layoutChangeAnimationsFinished()
{
    DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *castedParent = this->getCastedParentLayout();
    if(castedParent == NULL)
    {
        WLog("Parent is not the type we need.");
        return;
    }

    //castedParent->setLayoutUpdateBlocked(false, false);

    this->_hideAndSendSignalAboutDeletedItems();

    //
    // THIS IS REQUIRED !!! -> the changeSnapshot method will use it and may modify it
    //  for myself (Viktor): do a favor for yourself - don't modify this, or at least check whether it's used anywhere else as well in this code before optimization
    isProcessingSnapshotChangeFlag = false;

#if 0
    //
    // chech for another layout change (happened during the animation-block)
    if(!snapshotQueue.isEmpty())
    {
        //DLog(" ! there are other queued changes");
        // another snapshot is in the queue -> do it
        SnapshotQueueItem snapshotItem = snapshotQueue.dequeue();
        this->changeSnapshot(snapshotItem.getSourceSnapshot(), snapshotItem.getTargetSnapshot());
    }
#endif

    // ! this is an optimized solution
    //      it eliminates the quene and compresses the change to only
    //      one SnapshotChange item - it's source is the first unprocessed source
    //      and it's target is the last unprocessed target snapshot
    if(isCompactSnapshotChangeAvailable)
    {
        isCompactSnapshotChangeAvailable = false;
        isCompactSnapshotSourceNotYetDefined = true;
        this->changeSnapshot(compactSnapshotChange.getSourceSnapshot(), compactSnapshotChange.getTargetSnapshot());
    }

    if(!isProcessingSnapshotChangeFlag)
    {
        // no more snapshots, hurray!!
#ifdef ENABLE_SNAPSHOT_HANDLING_LOGGING
        DLog(" no more changes in the queue");
#endif
        castedParent->setLayoutUpdateBlocked(false, true);
        castedParent->update();

        Q_EMIT layoutChangeFinished();
    }
}

DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *SameSizeLayoutSnapshotChangeDelegateBase::getCastedParentLayout()
{
    return dynamic_cast<DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *>(this->parent());
}

SameSizeHorizontalLayoutingInformations SameSizeLayoutSnapshotChangeDelegateBase::getLayoutingInformations() {
    DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *castedParent = this->getCastedParentLayout();
    if(castedParent == NULL)
    {
        WLog("Parent is not the type we need.");
        return SameSizeHorizontalLayoutingInformations::CreateEmpty();
    }

    return castedParent->getLayoutingInformations();
}

void SameSizeLayoutSnapshotChangeDelegateBase::_addToCurrentlyHidingItems(QWidget *widget) {
    this->_currentlyDeletingItems << widget;
}

void SameSizeLayoutSnapshotChangeDelegateBase::_hideAndSendSignalAboutDeletedItems() {
    int cnt = _currentlyDeletingItems.size();
    for(int i = 0; i < cnt; i++) {
        QWidget *item = _currentlyDeletingItems[i];
        item->hide();
        Q_EMIT itemHiddenAndSafeToRemove(item);
    }

    this->_currentlyDeletingItems.clear();
}

#if 0
void SameSizeSnapshotChangeDelegateBase::_clearCurrentlyDeletingItemsList() {
    this->_currentlyDeletingItems.clear();
}
#endif

void SameSizeLayoutSnapshotChangeDelegateBase::setShowHideAnimationDuration(int durationInMilliseconds) {
    this->showHideAnimationDuration = durationInMilliseconds;
}

void SameSizeLayoutSnapshotChangeDelegateBase::setMoveAnimationDuration(int durationInMilliseconds) {
    this->moveAnimationDuration = durationInMilliseconds;
}

int SameSizeLayoutSnapshotChangeDelegateBase::getShowHideAnimationDuration() const {
    return this->showHideAnimationDuration;
}
int SameSizeLayoutSnapshotChangeDelegateBase::getMoveAnimationDuration() const {
    return this->moveAnimationDuration;
}

void SameSizeLayoutSnapshotChangeDelegateBase::addMoveAnimationToCurrentAnimationGroup(QWidget *widgetToMove, QPoint oldPos, QPoint newPos)
{
    QPropertyAnimation *animation = new QPropertyAnimation(widgetToMove, "geometry");
    animation->setDuration(this->getMoveAnimationDuration());
    animation->setStartValue(QRect(oldPos, widgetToMove->geometry().size()));
    animation->setEndValue(QRect(newPos, widgetToMove->geometry().size()));
    animation->setEasingCurve(QEasingCurve::InOutQuart);

    this->currentAnimGroup->addAnimation(animation);
}

void SameSizeLayoutSnapshotChangeDelegateBase::addHideAnimationToCurrentAnimationGroup(QWidget *widgetToHide)
{
    QPropertyAnimation *presenterMaxSizeAnim = new QPropertyAnimation(widgetToHide, "maximumSize");
    presenterMaxSizeAnim->setDuration(this->getShowHideAnimationDuration());
    presenterMaxSizeAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
    presenterMaxSizeAnim->setStartValue(widgetToHide->geometry().size());
    presenterMaxSizeAnim->setEndValue(QSize(0, 0));

    QPropertyAnimation *presenterMinSizeAnim = new QPropertyAnimation(widgetToHide, "minimumSize");
    presenterMinSizeAnim->setDuration(this->getShowHideAnimationDuration());
    presenterMinSizeAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
    presenterMinSizeAnim->setStartValue(widgetToHide->geometry().size());
    presenterMinSizeAnim->setEndValue(QSize(0, 0));

    this->currentAnimGroup->addAnimation(presenterMinSizeAnim);
    this->currentAnimGroup->addAnimation(presenterMaxSizeAnim);
}

void SameSizeLayoutSnapshotChangeDelegateBase::addShowAnimationToCurrentAnimationGroup(QWidget *widgetToShow,
                                                                                 SameSizeHorizontalLayoutingInformations layoutingInformations)
{
    QPropertyAnimation *presenterMaxSizeAnim = new QPropertyAnimation(widgetToShow, "maximumSize");
    presenterMaxSizeAnim->setDuration(this->getShowHideAnimationDuration());
    presenterMaxSizeAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
    presenterMaxSizeAnim->setStartValue(QSize(0, 0));
    presenterMaxSizeAnim->setEndValue(layoutingInformations.getItemSize());

    QPropertyAnimation *presenterMinSizeAnim = new QPropertyAnimation(widgetToShow, "minimumSize");
    presenterMinSizeAnim->setDuration(this->getShowHideAnimationDuration());
    presenterMinSizeAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
    presenterMinSizeAnim->setStartValue(QSize(0, 0));
    presenterMinSizeAnim->setEndValue(layoutingInformations.getItemSize());

    this->currentAnimGroup->addAnimation(presenterMinSizeAnim);
    this->currentAnimGroup->addAnimation(presenterMaxSizeAnim);
}

void SameSizeLayoutSnapshotChangeDelegateBase::_addAnimationToGroup(QPropertyAnimation *anim) {
    this->currentAnimGroup->addAnimation(anim);
}

void SameSizeLayoutSnapshotChangeDelegateBase::startCurrentAnimationGroup()
{
    if( currentAnimGroup->animationCount() > 0 )
    {
        currentAnimGroup->start();
    }
    else
    {
        // we didn't add any animations so delete this immediatelly
        isProcessingSnapshotChangeFlag = false;
    }
}

void SameSizeLayoutSnapshotChangeDelegateBase::clearCurrentAnimationGroup()
{
    this->currentAnimGroup->stop();
    this->currentAnimGroup->clear();
}
