#include "browsersidebarcontainer.h"

#include <QString>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

#include "Browser/Sidebar/browsersidebartextwidget.h"

BrowserSideBarContainer::BrowserSideBarContainer(QWidget *parent) :
    DataSourceWidget(DATASOURCE_WIDGET_WITH_VERTICAL_LAYOUT, parent)
{
    this->setFixedSize(200, 60);

    addNewObject(new BrowserSideBarTextWidget("Login", this));
    addNewObject(new BrowserSideBarTextWidget("Register", this));
    addNewObject(new BrowserSideBarTextWidget("Lost password?", this));
}
