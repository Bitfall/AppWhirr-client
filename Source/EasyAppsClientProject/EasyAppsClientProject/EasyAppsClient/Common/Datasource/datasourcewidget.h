#ifndef DATASOURCEWIDGET_H
#define DATASOURCEWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLayout;
QT_END_NAMESPACE

class DataSourceElementWidget;

#define DATASOURCE_WIDGET_WITH_VERTICAL_LAYOUT      0
#define DATASOURCE_WIDGET_WITH_HORIZONTAL_LAYOUT    1
#define DATASOURCE_WIDGET_WITH_GRID_LAYOUT          2

class DataSourceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataSourceWidget(int layoutType = 0, QWidget *parent = 0);

    QList<QWidget*> getContainedItems();

    QLayout*                    layout;

public Q_SLOTS:
    void widgetSelected(QWidget* widget);

protected:
    void addNewObject(QWidget* widget);
    void addNewObject(QWidget* widget, int row, int column);

private:
    DataSourceElementWidget*    selectedWidget;

    QList<QWidget*>             containedItems;
};

#endif // DATASOURCEWIDGET_H
