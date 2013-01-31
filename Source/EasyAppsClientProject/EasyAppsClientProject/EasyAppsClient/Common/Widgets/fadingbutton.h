#ifndef FADINGBUTTON_H
#define FADINGBUTTON_H

#include <QPushButton>
#include "Common/fadingitemdelegate.h"

class FadingButton : public QPushButton, public FadingItemDelegate::FadingPerformerDelegateInterface
{
    Q_OBJECT
public:
    explicit FadingButton(QWidget *parent = 0);

public Q_SLOTS:
    //
    // show/hide (fade-in/out) slots
    void showWithAnimation();
    void hideWithAnimation();


    // ---------------------------------------------------------------
    // --- FadingPreformerDelegateInterface implementation methods ---
public:
    void performNormalPaint(QPaintEvent *e);
    void performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e);
    QWidget *getPresenterWidget();

protected:
    void paintEvent(QPaintEvent *);

private:
    FadingItemDelegate *_fadingItemDelegate;
};

#endif // IMAGEBASEDBUTTON_H
