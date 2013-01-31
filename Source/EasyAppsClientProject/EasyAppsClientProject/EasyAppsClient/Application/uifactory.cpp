#include "uifactory.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSpacerItem>
#include <QPushButton>

using namespace AppWhirr;

UiFactory::UiFactory()
{
}

QVBoxLayout *UiFactory::createDefaultVBoxLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    return layout;
}

QHBoxLayout *UiFactory::createDefaultHBoxLayout()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    return layout;
}

QWidget *UiFactory::createWidgetWithZeroContentMargins()
{
    QWidget *widget = new QWidget;
    widget->setContentsMargins(0, 0, 0, 0);
    return widget;
}

QSpacerItem *UiFactory::createHorizontalSpacer() {
    return new QSpacerItem(0, 0, QSizePolicy::Expanding);
}

QSpacerItem *UiFactory::createVerticalSpacer() {
    return new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
}
