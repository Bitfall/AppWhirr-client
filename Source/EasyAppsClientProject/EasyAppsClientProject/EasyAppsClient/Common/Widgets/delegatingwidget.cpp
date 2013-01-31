#include "delegatingwidget.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>

DelegatingWidget::DelegatingWidget(QWidget *parent) :
    QWidget(parent)
{
}

void DelegatingWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    Q_EMIT resizeHappened(e);
}

void DelegatingWidget::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

//    QWidget::paintEvent(e);
}

