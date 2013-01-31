#ifndef APPWHIRRLICENCEPAGE_H
#define APPWHIRRLICENCEPAGE_H

#include "Common/Page/pagewidgetbase.h"

class AppwhirrLicencePage : public PageWidgetBase
{
    Q_OBJECT
public:
    explicit AppwhirrLicencePage(QString pageId, QWidget *parent = 0);

    // -------------------------------------
    // --- PageWidgetBase implementation ---

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

#endif // APPWHIRRLICENCEPAGE_H
