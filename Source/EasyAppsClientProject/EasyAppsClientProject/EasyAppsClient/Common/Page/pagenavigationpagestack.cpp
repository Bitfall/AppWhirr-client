#include "pagenavigationpagestack.h"

#include "pagewidgetbase.h"

PageNavigationPageStack::PageNavigationPageStack(PageWidgetBase *rootPage) :
        rootPage(rootPage),
        isRootInitialized(false)
{
}

void PageNavigationPageStack::pushPage(PageWidgetBase *page)
{
    stackedSubPages.append(page);
}

PageWidgetBase *PageNavigationPageStack::popPage()
{
    if(stackedSubPages.isEmpty())
    {
        return NULL;
    }

    PageWidgetBase *ret = stackedSubPages.last();
    stackedSubPages.removeLast();
    return ret;
}

PageWidgetBase *PageNavigationPageStack::getCurrentTopPage()
{
    if(stackedSubPages.isEmpty())
    {
        return rootPage;
    }

    return stackedSubPages.last();
}

PageWidgetBase *PageNavigationPageStack::getRootPage()
{
    return rootPage;
}

QList<PageWidgetBase *> PageNavigationPageStack::getAllPages()
{
    QList<PageWidgetBase *> retList;
    retList << rootPage;

    int subPageCnt = stackedSubPages.size();
    for(int i = 0; i < subPageCnt; i++)
    {
        retList << stackedSubPages[i];
    }

    return retList;
}

QList<PageWidgetBase *> PageNavigationPageStack::getSubPages()
{
    return stackedSubPages;
}

bool PageNavigationPageStack::getIsRootInitialized()
{
    return isRootInitialized;
}

void PageNavigationPageStack::setRootInitialized()
{
    isRootInitialized = true;
}

int PageNavigationPageStack::getSubPagesCount() {
    return stackedSubPages.size();
}

int PageNavigationPageStack::getAllPagesCount() {
    return this->getSubPagesCount() + 1; // + 1 for the root page
}

int PageNavigationPageStack::indexOfPageInStack(QString pageItemId)
{
    if(rootPage->getPageId() == pageItemId) {
        // found, it's the root page
        return 0;
    }

    int subPageCnt = stackedSubPages.size();
    int i = 0;
    bool isFound = false;
    for(; i < subPageCnt && !isFound; )
    {
        if( stackedSubPages[i]->getPageId() == pageItemId ) {
            isFound = true;
        }
        else {
            ++i;
        }
    }

    if(i < subPageCnt) {
        return i + 1; // +1 for the root page, because the loop only counted the sub-pages but not the root
    }

    // not found
    return -1;
}
