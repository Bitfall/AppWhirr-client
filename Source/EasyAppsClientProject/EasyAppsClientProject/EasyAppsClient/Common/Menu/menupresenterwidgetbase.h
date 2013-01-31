#ifndef MENUPRESENTERWIDGETBASE_H
#define MENUPRESENTERWIDGETBASE_H

#include <QWidget>

class MenuItemWidget;
class MenuItemHolderWidgetBase;

class MenuPresenterWidgetBase : public QWidget
{
public:
    explicit MenuPresenterWidgetBase(QWidget *parent = 0) : QWidget(parent) {}

    virtual void addRootMenuItemHolderWidget(MenuItemHolderWidgetBase *widget) = 0;
    virtual MenuItemHolderWidgetBase *createMenuItemHolderWidgetByMenuItemWidget(MenuItemWidget *menuItemWidget) = 0;

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // MENUPRESENTERWIDGETBASE_H
