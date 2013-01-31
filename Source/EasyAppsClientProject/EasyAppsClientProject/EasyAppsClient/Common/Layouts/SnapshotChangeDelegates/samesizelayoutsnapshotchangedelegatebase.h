#ifndef SAMESIZELAYOUTSNAPSHOTCHANGEDELEGATEBASE_H
#define SAMESIZELAYOUTSNAPSHOTCHANGEDELEGATEBASE_H

#include <QObject>

#include "snapshotqueueitem.h"
#include "Common/Sorting/widgetlistsnapshot.h"
#include "Common/Layouts/layoutinginformations.h"

QT_BEGIN_NAMESPACE
class QParallelAnimationGroup;
class QPropertyAnimation;
QT_END_NAMESPACE

class DynamicDelegateBasedHighPerformanceSameSizeLayoutBase;

class SameSizeLayoutSnapshotChangeDelegateBase : public QObject
{
    Q_OBJECT
public:
    explicit SameSizeLayoutSnapshotChangeDelegateBase(QObject *parent = 0);

//    virtual void setIconPresenterListDelegate(IconPresenterListDelegate *delegate) = 0;

    void setShowHideAnimationDuration(int durationInMilliseconds);
    void setMoveAnimationDuration(int durationInMilliseconds);

Q_SIGNALS:
    void itemHiddenAndSafeToRemove(QWidget *item);
    // possibly another Q_SIGNAL: itemWillBeShown ?

    void layoutChangeStarted(QSize currentOptimalSize, QSize predictedOptimalSizeAfterLayoutChange);
    void layoutChangeFinished();

public Q_SLOTS:
    void changeSnapshot(WidgetListSnapshot sourceSnapshot, WidgetListSnapshot targetSnapshot);

protected Q_SLOTS:
    void layoutChangeAnimationsFinished();

protected:
    DynamicDelegateBasedHighPerformanceSameSizeLayoutBase *getCastedParentLayout();
    SameSizeHorizontalLayoutingInformations getLayoutingInformations();

    //
    void _addToCurrentlyHidingItems(QWidget *widget);
    //
    // ! this will hide the deleted items during the current / last snapshot change
    // ! and it will also clear the list of this marked items
    void _hideAndSendSignalAboutDeletedItems();
//    void _clearCurrentlyDeletingItemsList();

    //
    // this implementation will be called from the changeSnapshot Q_SLOT
    //  and it's responsibility is to make the move, hide, show animations
    //  and add them to the current animation-group via addXXXAnimationToCurrentAnimationGroup
    void _changeSnapshotImplementation(WidgetListSnapshot sourceSnapshot, WidgetListSnapshot targetSnapshot,
                                               SameSizeHorizontalLayoutingInformations layoutingInformations);

protected:
    bool isProcessingSnapshotChangeFlag;
    //QQueue<SnapshotQueueItem> snapshotQueue;

    bool isCompactSnapshotChangeAvailable;   // indicates that a compact-snapshot-change is available for processing
    bool isCompactSnapshotSourceNotYetDefined; // if true then no compact-snapshot-change source is set yet
    // ! this is an optimized solution
    //      it eliminates the quene and compresses the change to only
    //      one SnapshotChange item - it's source is the first unprocessed source
    //      and it's target is the last unprocessed target snapshot
    SnapshotQueueItem compactSnapshotChange;


    int getShowHideAnimationDuration() const;
    int getMoveAnimationDuration() const;

    virtual void addMoveAnimationToCurrentAnimationGroup(QWidget *widgetToMove, QPoint oldPos, QPoint newPos);
    virtual void addHideAnimationToCurrentAnimationGroup(QWidget *widgetToHide);
    virtual void addShowAnimationToCurrentAnimationGroup(QWidget *widgetToShow, SameSizeHorizontalLayoutingInformations layoutingInformations);

    void _addAnimationToGroup(QPropertyAnimation *anim);

private:
    void startCurrentAnimationGroup();
    void clearCurrentAnimationGroup();

private:
    // this list contains the widgets which will be deleted during the current (!) snapshot change
    QList<QWidget *> _currentlyDeletingItems;

    int showHideAnimationDuration;
    int moveAnimationDuration;

    QParallelAnimationGroup *currentAnimGroup;
};

#endif // SAMESIZELAYOUTSNAPSHOTCHANGEDELEGATEBASE_H
