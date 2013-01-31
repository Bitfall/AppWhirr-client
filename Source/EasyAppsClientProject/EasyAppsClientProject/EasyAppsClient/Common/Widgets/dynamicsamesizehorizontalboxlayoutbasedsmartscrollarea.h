#ifndef DYNAMICSAMESIZEHORIZONTALBOXLAYOUTBASEDSMARTSCROLLAREA_H
#define DYNAMICSAMESIZEHORIZONTALBOXLAYOUTBASEDSMARTSCROLLAREA_H

#include <QScrollArea>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Common/Sorting/widgetlistsnapshot.h"

class DynamicSameSizeHorizontalBoxLayout;
class MouseGrabScrollDelegate;
class WidgetListSnapshotHandlerDelegate;
class IdBasedSelectableAndHighlightableWidgetBase;
class IdBasedWidgetCollection;
class IdBasedWidgetBase;
class SameSizeLayoutSnapshotChangeDelegateBase;

//
// it can only manage IdBasedSelectableAndHighlightableWidgetBase derived widgets
//  also it's based on same-size layout and layout-snapshot-change delegate so the items have to be same-size
//  -> this is an important performance optimization
class DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea(QSize itemSize, SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate, bool isCenterizeItemsHorizontally, QWidget *parent = 0);

    //
//    void addWidgetToList(IdBasedSelectableAndHighlightableWidgetBase *widgetToAdd, bool isPresentItAsWell);
    //
    // adds the widget to the layout and to the current working snapshot as well
    //
    // if isInitializedAndAddedPreviously is true, then it won't install event filter, connect required signals and won't add it to the layout
    //  so set it "true" if it's added previously
    //  basicly: if it's true then it will only add it to the working-snapshot but won't initialize it for internal usage
    void addIdBasedWidgetToCurrentWorkingSnapshot(IdBasedSelectableAndHighlightableWidgetBase *widgetToAdd, bool isInitializedAndAddedPreviously = false);
    //
    // it adds the widget to the layout but not to the current working snapshot
    //
    // if isInitializedAndAddedPreviously is true, then it won't install event filter, connect required signals and won't add it to the layout
    //  so set it "true" if it's added previously
    //  basicly: if it's true then it will only add it to the working-snapshot but won't initialize it for internal usage
    void addIdBasedWidgetToLayout(IdBasedSelectableAndHighlightableWidgetBase *widget, bool isInitializedAndAddedPreviously = false);


    void setContentMarginsForDynamicLayout(int left, int top, int right, int bottom);

    //
    // commit handling
    QList<QWidget *> getCurrentWorkingSnapshotAsWidgetList() const;
    void clearCurrentWorkingSnapshot();
    QList<IdBasedWidgetBase *> getCurrentWorkingSnapshot() const;
    IdBasedWidgetBase *getItemFromCurrentWorkingSnapshotByItemId(QString itemId) const;
    void _commitCurrentWorkingSnapshot(); // probably you don't want to use this directly, but call _commitCurrentWorkingSnapshotWithThisOrder instead (after a sorting)

    IdBasedWidgetCollection *__getIdBasedWidgetCollection() const; // DEPRECATED - use getCurrentWorkingSnapshot and commitCurrentWorkingSnapshot methods instead of directly communicating with the working-snapshot-holder object
    WidgetListSnapshotHandlerDelegate *getSnapshotHandlerDelegate() const;

    //
    // will activate the currently selected item or the first one, of none selected - like the user clicked it / activated it
    void activateCurrentItem();

//    QSize sizeHint() const;

Q_SIGNALS:
    void itemActivatedSignal(IdBasedSelectableAndHighlightableWidgetBase *source);
    void itemSelectedSignal(IdBasedSelectableAndHighlightableWidgetBase *item);
    void itemDeselectedSignal(IdBasedSelectableAndHighlightableWidgetBase *item);

//    void mouseEnteredToItemSignal(IdBasedSelectableAndHighlightableWidgetBase *source);
//    void mouseLeavedFromItemSignal(IdBasedSelectableAndHighlightableWidgetBase *source);

public Q_SLOTS:
    void _commitSnapshotWithThisOrderAndSetAsCurrentWorking(WidgetListSnapshot sortingResultSnapshot); // the snapshot's items have to be added to the working-snapshot before you call this - if not, they won't be added
    void ensureCurrentSelectedItemCenterizedInScrollArea();
    void ensureItemCenterizedInScrollArea(QString itemId);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void itemActivated(IdBasedSelectableAndHighlightableWidgetBase *source);
    void itemSelected(QWidget *item);
    void itemDeselected(QWidget *item);

    void mouseEnteredToItem(IdBasedSelectableAndHighlightableWidgetBase *source);
    void mouseLeavedFromItem(IdBasedSelectableAndHighlightableWidgetBase *source);

    void _updateContentSizeByLastGivenWidth();
    void _layoutSnapshotChangeStarted(QSize currentOptimalSize, QSize predictedOptimalSizeAfterLayoutChange);

private:

    void _updateContentSizeByWidthToFit(int targetContentWidth);
    void resizeContentWidgetAndUpdateIt(QSize newSize);

    //
    // will return the selected item, or tries to get the first one if no one is selected
    //  returns NULL if neither of them could be retrieved
    IdBasedSelectableAndHighlightableWidgetBase *getCurrentSelectedItemOrTheFirst() const;
    IdBasedSelectableAndHighlightableWidgetBase *getItemByIdFromWorkingSnapshotOrTheFirstInCommitedSnapshot(QString itemId) const;
    IdBasedSelectableAndHighlightableWidgetBase *getItemByIdFromCurrentWorkingSnapshotOrNull(QString itemId) const;

    // mouse
    bool processMouseButtonPressEvent(QObject *obj, QMouseEvent *e);
    bool processMouseMoveEvent(QObject *obj, QMouseEvent *e);
    bool processMouseButtonReleaseEvent(QObject *obj, QMouseEvent *e);
    // keyboard
    bool processKeyPressEvent(QKeyEvent *event);
    bool processKeyReleaseEvent(QKeyEvent *event);
    // keyboard event filter
    bool processKeyPressEventFilter(QKeyEvent *keyEvent);
    // mouse event filter
    bool processMouseWheelEventFilter(QWheelEvent *wheelEvent);

private:
    DynamicSameSizeHorizontalBoxLayout* contentLayout;

    IdBasedWidgetCollection *_currentWorkingSnapshotItems; // contains the current working-snapshot
//    IdBasedWidgetCollection *_lastCommitedSnapshotItems;
    MouseGrabScrollDelegate *mouseGrabScrollDelegate;

    QWidget *contentWidget;
//    int _lastGivenContentWidth;
    QSize _lastGivenContentSize;
};

#endif // DYNAMICSAMESIZEHORIZONTALBOXLAYOUTBASEDSMARTSCROLLAREA_H
