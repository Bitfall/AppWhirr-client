#include "browsersidebartextwidget.h"

#include <QLabel>

BrowserSideBarTextWidget::BrowserSideBarTextWidget(const QString& title, QWidget *parent) :
    DataSourceElementWidget(parent)
{
    this->setFixedSize(200, 20);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setText(title);
    label->setFixedSize(this->rect().size());
}
