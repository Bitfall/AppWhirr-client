#include "datasourceelementwidget.h"

#include <QMouseEvent>

DataSourceElementWidget::DataSourceElementWidget(QWidget *parent) :
    QWidget(parent)
{
    connect(this, SIGNAL(widgetSelected(QWidget*)), parent, SLOT(widgetSelected(QWidget*)));
    connect(this, SIGNAL(widgetSelected(QWidget*)), this, SLOT(widgetSelectedSlot(QWidget*)));
    connect(this, SIGNAL(widgetDeselected()), this, SLOT(widgetDeselectedSlot()));
}

void DataSourceElementWidget::mousePressEvent (QMouseEvent* event)
{
    Q_EMIT this->widgetSelected(this);
}

void DataSourceElementWidget::widgetSelectedSlot(QWidget* widget) {
}

void DataSourceElementWidget::widgetDeselectedSlot() {
}
