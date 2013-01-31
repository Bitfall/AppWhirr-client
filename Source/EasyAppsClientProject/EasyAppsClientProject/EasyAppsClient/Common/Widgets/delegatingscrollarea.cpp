#include "delegatingscrollarea.h"

DelegatingScrollArea::DelegatingScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
}

void DelegatingScrollArea::resizeEvent(QResizeEvent *e)
{
    QScrollArea::resizeEvent(e);
    Q_EMIT scrollAreaResized(e);
}
