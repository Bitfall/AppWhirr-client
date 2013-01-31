#include "widgetinlayouthelper.h"

WidgetInLayoutHelper::WidgetInLayoutHelper()
{
}

void WidgetInLayoutHelper::replaceWidgetInLayoutWithWidget(QWidget *sourceWidget, QWidget *targetWidget, QBoxLayout *layout)
{
    int indexWhereToPutTheTargetWidget = layout->indexOf(sourceWidget);
    if(indexWhereToPutTheTargetWidget == -1)
    {
        qDebug(" ! The source widget is not found in the layout - cannot switch to it");
        return;
    }

    // remove the source widget
    layout->removeWidget(sourceWidget);
    sourceWidget->setParent(NULL);

    // insert the new widget to the source widget's place
    layout->insertWidget(indexWhereToPutTheTargetWidget, targetWidget);
    targetWidget->show();
}

QRect WidgetInLayoutHelper::calculateWidgetGeometryInLayout(QWidget *widget, QBoxLayout *layout, int indexWhereToInsertTheWidget)
{
    // 1) insert the widget to it's place
    layout->insertWidget(indexWhereToInsertTheWidget, widget);
    widget->show();
    widget->updateGeometry();
    widget->adjustSize();
    layout->update();
    layout->invalidate();
    layout->activate();
    // get the widget's position now
    QPoint widgetPos = widget->geometry().topLeft();

    // 2) then remove it
    layout->removeWidget(widget);
    widget->setParent(NULL);
    widget->hide(); // without this the page appears as a separate window for a second

    // 3) now the widget's size is calculated properly, so combine with the position
    QRect widgetGeom = QRect(widgetPos, widget->geometry().size());

    return widgetGeom;
}

QSize WidgetInLayoutHelper::calculateWidgetSizeInLayout(QWidget *widget, QBoxLayout *layout)
{
    // first, insert the widget and update it's geometry and also force the layout to update / recalculate
    layout->insertWidget(0, widget);
    widget->show();
    widget->updateGeometry();
    widget->adjustSize();
    layout->update();
    layout->invalidate();
    layout->activate();

    // then remove it
    layout->removeWidget(widget);
    widget->setParent(NULL);
    widget->hide(); // without this the page appears as a separate window for a second

    // not the widget's size is calculated, return it
    return widget->geometry().size();
}
