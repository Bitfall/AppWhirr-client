#ifndef PLAINVERTICALMENUPRESENTERWIDGET_H
#define PLAINVERTICALMENUPRESENTERWIDGET_H

#include "menupresenterwidgetbase.h"
#include "Common/Widgets/animatedtwofixedsizewidgetbase.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

class DelegatingWidget;

//
// !!!
// to change the items in the menu
//  use ONLY the activateCurrentItemPathBySnapshotChange, it will manage the transitions safely (hopefully)
class PlainVerticalMenuPresenterWidget : public MenuPresenterWidgetBase
{
    Q_OBJECT
public:
    explicit PlainVerticalMenuPresenterWidget(QWidget *parent = 0);

    void addRootMenuItemHolderWidget(MenuItemHolderWidgetBase *menuItemHolderWidget);
    MenuItemHolderWidgetBase *createMenuItemHolderWidgetByMenuItemWidget(MenuItemWidget *menuItemWidget);

protected:
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void _menuContentHolderWidgetResized(QResizeEvent *e);


private:
    DelegatingWidget *_menuContentHolderWidget;
    QVBoxLayout *_menuContentLayout;
    QScrollArea *_menuContentScrollArea;
};

#endif // PLAINVERTICALMENUPRESENTERWIDGET_H
