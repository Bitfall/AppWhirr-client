#include "intelligentseparatedmultifunctionalbuttonoptionitemwidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "intelligentseparatedmultifunctionalbuttonoptionitemmodel.h"

IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget::IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *relatedOptionItem, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, QWidget *parent) :
    AnimatedTwoFixedSizeWidgetBase(1000, collapsedSize, expandedSize, isExpandedByDefault, true, true, parent),
    relatedOptionItem(relatedOptionItem)
{
    this->relatedOptionItem->setParent(this);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    titleLabel = new QLabel;
    titleLabel->setFont(relatedOptionItem->getPreferedFont());
    titleLabel->setText(relatedOptionItem->getTitleText());
    mainLayout->addWidget(titleLabel);
}

IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget::getRelatedOptionItemModel() const
{
    return this->relatedOptionItem;
}

bool IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget::event(QEvent *e) {
    if(e->type() == QEvent::MouseButtonRelease) {
        this->relatedOptionItem->activateOption();
        e->accept();
        return true;
    }
//    else if(e->type() == QEvent::MouseButtonRelease) {
//        e->accept();
//        return true;
//    }

    return AnimatedTwoFixedSizeWidgetBase::event(e);
}

//void IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget::mousePressEvent(QMouseEvent * event)
//{
//    if(event->button() == Qt::LeftButton) {
//        this->relatedOptionItem->activateOption();
//        event->accept();
//    }

//    AnimatedTwoFixedSizeWidgetBase::mousePressEvent(event);
//}
