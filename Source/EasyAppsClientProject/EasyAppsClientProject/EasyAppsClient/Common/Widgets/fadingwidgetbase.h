#ifndef FADINGWIDGETBASE_H
#define FADINGWIDGETBASE_H

#include <QWidget>


#include "Common/fadingiteminterface.h"
#include "Common/fadingitemdelegate.h"


/*! This is a base widget to support fade-in / fade-out animations automatically.

    Basically this is the bare implementation of \a FadingItemInterface with the use of \a FadingItemDelegate
*/
class FadingWidgetBase : public QWidget, public FadingItemInterface, public FadingItemDelegate::FadingPerformerDelegateInterface
{
    Q_OBJECT

public:
    explicit FadingWidgetBase(quint32 fadeDurationMilliseconds, QWidget *parent = 0);

    // --------------------------------------------------
    // --- FadingItemInterface implementation methods ---
    void startFadeInAnimation();
    void startFadeOutAnimation();

    // ---------------------------------------------------------------
    // --- FadingPreformerDelegateInterface implementation methods ---
    void performNormalPaint(QPaintEvent *e);
    void performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e);
    QWidget *getPresenterWidget();

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    FadingItemDelegate *_fadingItemDelegate;
};

#endif // FADINGWIDGETBASE_H
