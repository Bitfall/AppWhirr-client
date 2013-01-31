#ifndef PARALLELANIMATEDWIDGETREPLACERITEM_H
#define PARALLELANIMATEDWIDGETREPLACERITEM_H

#include <QWidget>
#include <QSize>
#include "Common/Widgets/crossfaderwidget.h"

class ParallelAnimatedWidgetReplacerItem
{
public:
    ParallelAnimatedWidgetReplacerItem(QWidget *sourceWidget, QWidget *targetWidget,
                                       QSize sourceWidgetSize, QSize targetWidgetSize,
                                       CrossFaderWidget *crossFaderWidget, bool isAlsoDeleteSourceWidget);

    QWidget *getSourceWidget();
    QWidget *getTargetWidget();

    QSize getSourceWidgetSize();
    void setSourceWidgetSize(QSize s);
    QSize getTargetWidgetSize();
    void setTargetWidgetSize(QSize s);

    CrossFaderWidget *getCrossFaderWidget();
    bool getIsAlsoDeleteSourceWidget();

private:
    QWidget *sourceWidget; // can be released when the replace starts
    QWidget *targetWidget;

    QSize sourceWidgetSize;
    QSize targetWidgetSize;

    CrossFaderWidget *crossFaderWidget;
    bool isAlsoDeleteSourceWidget;
};

#endif // PARALLELANIMATEDWIDGETREPLACERITEM_H
