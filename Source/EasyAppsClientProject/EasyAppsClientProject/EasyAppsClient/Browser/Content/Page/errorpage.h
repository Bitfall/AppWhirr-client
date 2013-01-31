#ifndef ERRORPAGE_H
#define ERRORPAGE_H

#include <QWidget>
#include "Common/Page/pagewidgetbase.h"

namespace Ui {
    class ErrorPage;
}

class ErrorPage : public PageWidgetBase
{
    Q_OBJECT

public:
    explicit ErrorPage(QString pageId, QWidget *parent = 0);
    ~ErrorPage();

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

private:
    Ui::ErrorPage *ui;
};

#endif // ERRORPAGE_H
