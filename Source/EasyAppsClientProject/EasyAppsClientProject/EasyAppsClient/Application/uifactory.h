#ifndef UIFACTORY_H
#define UIFACTORY_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QSpacerItem;
class QPushButton;
QT_END_NAMESPACE

namespace AppWhirr
{

class UiFactory
{
private:
    explicit UiFactory();

public:
    static QVBoxLayout *createDefaultVBoxLayout();
    static QHBoxLayout *createDefaultHBoxLayout();

    static QWidget *createWidgetWithZeroContentMargins();

    static QSpacerItem *createHorizontalSpacer();
    static QSpacerItem *createVerticalSpacer();
};

} // namespace

#endif // UIFACTORY_H
