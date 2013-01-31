#include "parallelanimatedwidgetreplaceritem.h"

ParallelAnimatedWidgetReplacerItem::ParallelAnimatedWidgetReplacerItem(QWidget *sourceWidget, QWidget *targetWidget,
                                                                       QSize sourceWidgetSize, QSize targetWidgetSize,
                                                                       CrossFaderWidget *crossFaderWidget, bool isAlsoDeleteSourceWidget) :
sourceWidget(sourceWidget),
targetWidget(targetWidget),
sourceWidgetSize(sourceWidgetSize),
targetWidgetSize(targetWidgetSize),
crossFaderWidget(crossFaderWidget),
isAlsoDeleteSourceWidget(isAlsoDeleteSourceWidget)
{
}

QWidget *ParallelAnimatedWidgetReplacerItem::getSourceWidget() {
    return this->sourceWidget;
}

QWidget *ParallelAnimatedWidgetReplacerItem::getTargetWidget() {
    return this->targetWidget;
}

QSize ParallelAnimatedWidgetReplacerItem::getSourceWidgetSize() {
    return this->sourceWidgetSize;
}

void ParallelAnimatedWidgetReplacerItem::setSourceWidgetSize(QSize s) {
    this->sourceWidgetSize = s;
}

QSize ParallelAnimatedWidgetReplacerItem::getTargetWidgetSize() {
    return this->targetWidgetSize;
}

void ParallelAnimatedWidgetReplacerItem::setTargetWidgetSize(QSize s) {
    this->targetWidgetSize = s;
}

CrossFaderWidget *ParallelAnimatedWidgetReplacerItem::getCrossFaderWidget() {
    return this->crossFaderWidget;
}

bool ParallelAnimatedWidgetReplacerItem::getIsAlsoDeleteSourceWidget() {
    return this->isAlsoDeleteSourceWidget;
}
