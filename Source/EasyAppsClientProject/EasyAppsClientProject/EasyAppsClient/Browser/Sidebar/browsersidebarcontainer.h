#ifndef BROWSERSIDEBARCONTAINER_H
#define BROWSERSIDEBARCONTAINER_H

#include <QWidget>
#include <QList>

#include "Common/Datasource/datasourcewidget.h"

QT_BEGIN_NAMESPACE
class QString;
class QPaintEvent;
QT_END_NAMESPACE

class BrowserSideBarContainer : public DataSourceWidget
{
    Q_OBJECT
public:
    explicit BrowserSideBarContainer(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
};

#endif // BROWSERSIDEBARCONTAINER_H
