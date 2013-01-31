#ifndef WIDGETINLAYOUTSWITCHER_H
#define WIDGETINLAYOUTSWITCHER_H

#include <QWidget>
#include <QBoxLayout>

class WidgetInLayoutHelper
{
private:
    WidgetInLayoutHelper();

public:
    //
    // removes the sourceWidget from the given layout and inserts the targetWidget to it's place
    //  ! the sourceWidget have to be in the layout
    //  ! the targetWidget have to be NOT in the layout
    static void replaceWidgetInLayoutWithWidget(QWidget *sourceWidget, QWidget *targetWidget, QBoxLayout *layout);

    //
    // calculates the size of a widget if it would be in the given layout
    //  ! the widget have to be NOT included in the layout
    //
    //  ? should it insert the widget into the layout to a specified position, or is a simple add enough?
    static QSize calculateWidgetSizeInLayout(QWidget *widget, QBoxLayout *layout);

    //
    // calculates the geometry of a widget if it would be in the given layout at a given position (index)
    //  ! the widget have to be NOT included in the layout
    static QRect calculateWidgetGeometryInLayout(QWidget *widget, QBoxLayout *layout, int indexWhereToInsertTheWidget);
};

#endif // WIDGETINLAYOUTSWITCHER_H
