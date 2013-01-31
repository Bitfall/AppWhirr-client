#ifndef DYNAMICSAMESIZELAYOUTBASE_H
#define DYNAMICSAMESIZELAYOUTBASE_H

#include <QLayout>
#include <QList>

//
// # caching
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
class SameSizeLayoutBase : public QLayout
{
    Q_OBJECT
public:
    explicit SameSizeLayoutBase(QSize _itemSize, QWidget *parent = 0);
    virtual ~SameSizeLayoutBase();


    void addWidget(QWidget *w);
    void addItem(QLayoutItem *item);
    QSize sizeHint() const;
    QSize minimumSize() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    virtual void setGeometry(const QRect &rect);

//    void setItemSize(QSize itemSize); // can be allowed, this is forbidden mainly because testing (currently this is only settable through the constructor)

//    void _recalculateOptimalSizeByLastReceivedWidth();

    //
    // it's a same-size layout so the position of an item can be
    //  calculated by it's index
    // ! this method will get/calculate the margins (can be time-consuming)
    // ! depends on the last-received rect
    QRect getItemGeometryByIndexAndCalculatedMargins(int indexOfItem) const;

    const QRect getCachedLastReceivedRect() const;
    QSize getCalculatedOptimalSize() const;
    QSize getCalculatedMinimalSize() const;

    //
    // force recalculate minimal and optimal sizes by the given width
    virtual void recalculateMinimalAndOptimalSizesByTotalLayoutWidth(int width) = 0;

protected:
    virtual QRect getItemGeometryByIndex(int indexOfItem, QMargins margins) const = 0;

    //
    // this will call the Implementation method, but can perform some caching before the actual layout-update
    void performLayoutUpdate(const QRect &rect, bool isOnlyCalculate = false);
    //
    // preform a layout geometry recalculation and update
    virtual void _performLayoutUpdateImplementation(const QRect &rect, bool isOnlyCalculate = false) = 0;
    //
    // shrinks the given rect with the current margins
    QRect calculateEffectiveRect(const QRect &rect) const;
    virtual QSize getItemSize() const;

protected:
    void setCalculatedMinimalAndOptimalSizes(QSize calculatedMinimalSize, QSize calculatedOptimalSize);


protected:
    QList<QLayoutItem *> _itemList;

private:
    QSize _itemSize;

    // caches
    QRect _lastReceivedRect;
    QSize _lastCalculatedOptimalSize;
    QSize _lastCalculatedMinimalSize;
};

#endif // DYNAMICSAMESIZELAYOUTBASE_H
