#include "fadingbutton.h"

#include <QPainter>
#include <QPaintEvent>

// config
#include "Settings/guiconfigurations.h"

#define FADE_DURATION_MILLISECONDS 500

FadingButton::FadingButton(QWidget *parent) :
    QPushButton(parent)
{
    this->setAttribute(Qt::WA_NoSystemBackground, true);

    this->_fadingItemDelegate = new FadingItemDelegate(this, FADE_DURATION_MILLISECONDS, this);
}

void FadingButton::paintEvent(QPaintEvent *e)
{
    this->_fadingItemDelegate->delegatePaintEventHere(e);
}

void FadingButton::showWithAnimation()
{
    this->_fadingItemDelegate->startFadeInAnimation();
}

void FadingButton::hideWithAnimation()
{
    this->_fadingItemDelegate->startFadeOutAnimation();
}

// ---------------------------------------------------------------
// --- FadingPreformerDelegateInterface implementation methods ---

void FadingButton::performNormalPaint(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
}

void FadingButton::performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e)
{
    QPainter painter(this);
    painter.setOpacity(requiredOpacity);
    painter.drawPixmap(this->rect(), pixmap);
}

QWidget *FadingButton::getPresenterWidget()
{
    return this;
}
