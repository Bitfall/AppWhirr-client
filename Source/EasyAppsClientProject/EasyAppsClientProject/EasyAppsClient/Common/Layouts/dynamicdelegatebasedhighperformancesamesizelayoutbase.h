#ifndef DYNAMICDELEGATEBASEDHIGHPERFORMANCESAMESIZELAYOUTBASE_H
#define DYNAMICDELEGATEBASEDHIGHPERFORMANCESAMESIZELAYOUTBASE_H

#include <QLayout>
#include <QList>
#include <QMap>

QT_BEGIN_NAMESPACE
class QLayoutItem;
QT_END_NAMESPACE

#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"
class WidgetListSnapshotHandlerDelegate;

#include "layoutinginformations.h"

//
// it will manage the sizeHint / minimumSize
//  these return cached-optimal-size
//
// this base class caches data:
//  - last received rect (sets when setGeometry is called)
//  - optimal size (in a horizontal box layout it's basicly the total width of the parent widget (last received rect's width) and height of calculated minimal size)
//  - minimal size
//
// ! if the itemSize's component is < 0 (e.g.: itemSize.width() < 0) then it will be full-sized (streched in that direction)
//      this will use the last-received (cached) rect
//
// ! use the dynamicContentMargins instead of the default contentsMargins - that one can cause troubles
//
class DynamicDelegateBasedHighPerformanceSameSizeLayoutBase : public QLayout
{
    Q_OBJECT
public:
    explicit DynamicDelegateBasedHighPerformanceSameSizeLayoutBase(WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate,
                                                                   SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate,
                                                                   QSize itemSize,
                                                                   QWidget *parent = 0);
    virtual ~DynamicDelegateBasedHighPerformanceSameSizeLayoutBase();


    void addWidget(QWidget *w);
    void addItem(QLayoutItem *item);
    QSize sizeHint() const;
    QSize minimumSize() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    virtual void setGeometry(const QRect &rect);

    QList<QWidget *> getCurrentLayoutDataSnapshot() const;

    void setDynamicContentMargins(int left, int top, int right, int bottom); // there were issues with the Qt set/getContentsMargins - it's a workaround
    void setDynamicContentMargins(QMargins margins);


    WidgetListSnapshotHandlerDelegate *getSnapshotHandlerDelegate() const;
    SameSizeLayoutSnapshotChangeDelegateBase *getSnapshotChangeDelegate() const;

//    void setItemSize(QSize itemSize); // can be allowed, this is forbidden mainly because testing (currently this is only settable through the constructor)


    //
    void setLayoutUpdateBlocked(bool isBlocked, bool isUpdateLayoutAfter = true);

    void _recalculateOptimalSizeByLastReceivedWidth();

    //
    // abstract
    virtual SameSizeHorizontalLayoutingInformations getLayoutingInformations() = 0;
    virtual void recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width) = 0;

    //
    // it's a same-size layout so the position of an item can be
    //  calculated by it's index
    // ! if the margins is null (margins.isNull() == true) then this method will get/calculate the margins (can be time-consuming)
    // ! these methods are depend on the last-received rect
    QRect getItemGeometryByIndexAndCalculatedMargins(int indexOfItem) const;
    virtual QRect getItemGeometryByIndex(int indexOfItem, QMargins margins) const = 0;

    const QRect getCachedLastReceivedRect() const;
    QSize getCalculatedOptimalSize() const;
    QSize getCalculatedMinimalSize() const;

protected Q_SLOTS:
    void snapshotChanged();

protected:
    //
    // abstract
    //
    // preforms a layout geometry recalculation and update
    // then returns the calculated desired size (best-fit)
    void performLayoutUpdate(const QRect &rect, bool isOnlyCalculate = false);
    virtual void _performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate = false) = 0;

    virtual bool isShouldBlockLayoutUpdateDuringAnimation() { return true; }

    QRect calculateEffectiveRect(const QRect &rect) const;

    void refreshDelegateConnections();
//    QSize getBiggestItemSize() const;
//    void recalculateBiggestItemSize();
    virtual QSize getItemSize() const;


    int getItemBaselinePositionY() const;
    int getItemBaselinePositionX() const;

    bool getIsLayoutUpdateBlocked() const;

    QMargins getDynamicContentMargins() const;


protected:
    void setCalculatedMinimalAndOptimalSizes(QSize calculatedMinimalSize, QSize calculatedOptimalSize);

private:
//    QList<QLayoutItem *> itemList;

    // delegates
    SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate;
    WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate;

    QSize itemSize;
    QMargins _dynamicContentMargins;

    // caches
    QRect _lastReceivedRect;
    QSize _lastCalculatedOptimalSize;
    QSize _lastCalculatedMinimalSize;
//    QSize _cachedBiggestItemSize;

    // flags
    bool isLayoutUpdateBlockedFlag;
};

#endif // DYNAMICDELEGATEBASEDHIGHPERFORMANCESAMESIZELAYOUTBASE_H
