#include "datasourcewidget.h"
#include "Common/Datasource/datasourceelementwidget.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

DataSourceWidget::DataSourceWidget(int layoutType, QWidget *parent) :
    QWidget(parent)
{
    selectedWidget = NULL;

    switch(layoutType) {
    case DATASOURCE_WIDGET_WITH_VERTICAL_LAYOUT:
        this->layout = new QVBoxLayout;
        break;
    case DATASOURCE_WIDGET_WITH_HORIZONTAL_LAYOUT:
        this->layout = new QHBoxLayout;
        break;
    case DATASOURCE_WIDGET_WITH_GRID_LAYOUT:
        this->layout = new QGridLayout;
        break;
    default:
        qDebug("Failed to add layout type: %d", layoutType);
    }

    if (this->layout) {
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        this->setLayout(this->layout);
    }
}

void DataSourceWidget::addNewObject(QWidget* widget) {
    this->layout->addWidget(widget);

    containedItems.append(widget);
}

void DataSourceWidget::addNewObject(QWidget* widget, int row, int column) {
    ((QGridLayout*)this->layout)->addWidget(widget, row, column);

    containedItems.append(widget);
}

void DataSourceWidget::widgetSelected(QWidget* widget) {
    if (selectedWidget) {
        Q_EMIT selectedWidget->widgetDeselectedSlot();
    }
    selectedWidget = (DataSourceElementWidget*)widget;
}

QList<QWidget*> DataSourceWidget::getContainedItems() {
    return containedItems;
}

