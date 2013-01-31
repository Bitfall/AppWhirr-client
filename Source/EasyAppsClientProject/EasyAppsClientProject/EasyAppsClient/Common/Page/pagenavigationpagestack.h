#ifndef PAGENAVIGATIONPAGESTACK_H
#define PAGENAVIGATIONPAGESTACK_H

class PageWidgetBase;

#include <QList>

//
// a stack is a hierarchial page-collection (stack)
// the root item is the first one (index 0, the first pushed to the stack), the first sub item is the second one, and so on
class PageNavigationPageStack
{
public:
    PageNavigationPageStack(PageWidgetBase *rootPage);

    PageWidgetBase *getRootPage();
    void pushPage(PageWidgetBase *page);
    PageWidgetBase *popPage();

    QList<PageWidgetBase *> getAllPages();
    QList<PageWidgetBase *> getSubPages();

    bool getIsRootInitialized();
    void setRootInitialized();

    int indexOfPageInStack(QString pageItemId);
    int getSubPagesCount();
    int getAllPagesCount();

    //
    // this can be the root or the topmost (last added) page from sub-pages
    PageWidgetBase *getCurrentTopPage();

private:
    PageWidgetBase *rootPage;
    QList<PageWidgetBase *> stackedSubPages;
    bool isRootInitialized;
};

#endif // PAGENAVIGATIONPAGESTACK_H
