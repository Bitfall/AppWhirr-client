#ifndef HELPPAGE_H
#define HELPPAGE_H

#include "Common/Page/pagewidgetbase.h"

namespace Ui {
    class HelpPage;
}

class MenuItemWidget;

class HelpPage : public PageWidgetBase
{
    Q_OBJECT
public:
    explicit HelpPage(QString pageId, QWidget *parent = 0);
    ~HelpPage();

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

public Q_SLOTS:
    void relatedMenuItemActivated(QString menuItemId);

private Q_SLOTS:
    void presentHelpItem2();

private:
    Ui::HelpPage *ui;
};

#endif // HELPPAGE_H
