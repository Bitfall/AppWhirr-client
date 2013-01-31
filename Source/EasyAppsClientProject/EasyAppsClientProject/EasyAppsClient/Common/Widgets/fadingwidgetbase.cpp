#include "fadingwidgetbase.h"

#include <QStylePainter>
#include <QPainter>
#include <QStyleOption>
#include <QPaintEvent>

FadingWidgetBase::FadingWidgetBase(quint32 fadeDurationMilliseconds, QWidget *parent) :
    QWidget(parent)
{
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->_fadingItemDelegate = new FadingItemDelegate(this, fadeDurationMilliseconds, this);
}

void FadingWidgetBase::paintEvent(QPaintEvent *e) {
    DLog("-- Paint Event");
    this->_fadingItemDelegate->delegatePaintEventHere(e);
}

// --------------------------------------------------
// --- FadingItemInterface implementation methods ---
void FadingWidgetBase::startFadeInAnimation()
{
    DLog("Start fade IN anim");
    this->_fadingItemDelegate->startFadeInAnimation();
}

void FadingWidgetBase::startFadeOutAnimation()
{
    DLog("Start fade OUT anim");
    this->_fadingItemDelegate->startFadeOutAnimation();
}

// ---------------------------------------------------------------
// --- FadingPreformerDelegateInterface implementation methods ---

void FadingWidgetBase::performNormalPaint(QPaintEvent *e)
{
    DLog("normal paint");
//    QPainter painter(this);
//    painter.fillRect(this->rect(), QColor(150, 200, 150));



    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FadingWidgetBase::performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e)
{
    DLog("paint with opacity: ") << requiredOpacity;
    QPainter painter(this);
    painter.setOpacity(requiredOpacity);
    painter.drawPixmap(this->rect(), pixmap);
}

QWidget *FadingWidgetBase::getPresenterWidget()
{
    return this;
}
