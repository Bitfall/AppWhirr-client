#ifndef WHATSNEWPAGE_H
#define WHATSNEWPAGE_H

#include "Common/Page/pagewidgetbase.h"

class WhatsNewPage : public PageWidgetBase
{
    Q_OBJECT

public:
    explicit WhatsNewPage(QString pageId, QWidget *parent = 0);

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

private Q_SLOTS:
    void _backButtonPressed();
};

#endif // WHATSNEWPAGE_H
