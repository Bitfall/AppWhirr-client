#include "samesizehorizontalflowlayoutbasedscrollarea.h"

#include <QScrollBar>

SameSizeHorizontalFlowLayoutBasedScrollArea::SameSizeHorizontalFlowLayoutBasedScrollArea(SameSizeLayoutBase *sameSizeLayout, QWidget *parent) :
    QScrollArea(parent),
    _contentHolderLayout(sameSizeLayout)
{
    _contentHolderLayout->setParent(this);
    this->setWidgetResizable(false);

    _contentHolderWidget = new QWidget;
    _contentHolderWidget->setLayout(_contentHolderLayout);
    this->setWidget(_contentHolderWidget);
}

void SameSizeHorizontalFlowLayoutBasedScrollArea::resizeEvent(QResizeEvent *e)
{
    QScrollArea::resizeEvent(e);

    _contentHolderLayout->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(this->contentsRect().width() - this->verticalScrollBar()->width());
    _contentHolderWidget->setMinimumSize(_contentHolderLayout->getCalculatedMinimalSize());
    _contentHolderWidget->resize(_contentHolderLayout->getCalculatedOptimalSize());
}

void SameSizeHorizontalFlowLayoutBasedScrollArea::addWidget(QWidget *widget) {
    _contentHolderLayout->addWidget(widget);
}
