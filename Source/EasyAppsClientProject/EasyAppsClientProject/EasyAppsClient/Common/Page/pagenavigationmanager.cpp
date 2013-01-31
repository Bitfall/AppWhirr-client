#include "pagenavigationmanager.h"

#include "pagenavigationpagestack.h"
#include <QDebug>
#include <QVBoxLayout>

PageNavigationManager::PageNavigationManager(PageTransitionManagerDelegateInterface *pageTransitionDelegate, QVBoxLayout *pageContentHolderLayout, QObject *parent) :
        QObject(parent),
    _contentLayout(pageContentHolderLayout),
    _pageTransitionDelegate(NULL)
{
    this->_setPageTransitionDelegate(pageTransitionDelegate);
}

PageNavigationManager::~PageNavigationManager()
{
    if(this->_pageTransitionDelegate != NULL) {
        delete this->_pageTransitionDelegate;
    }
}


void PageNavigationManager::_setPageTransitionDelegate(PageTransitionManagerDelegateInterface *pageTransitionDelegate)
{
    if(this->_pageTransitionDelegate) {
        delete _pageTransitionDelegate;
    }
    this->_pageTransitionDelegate = pageTransitionDelegate;
    connect(_pageTransitionDelegate, SIGNAL(transitionFinished(PageWidgetBase*,PageWidgetBase*,bool)), this, SLOT(_transitionFinished(PageWidgetBase*,PageWidgetBase*,bool)));
}

void PageNavigationManager::_setContentLayout(QVBoxLayout* layout)
{
    DLog("- set content layout");
    if(this->_contentLayout) {
//        delete this->_contentLayout; // will be deleted / released by it's parent widget / object
    }
    this->_contentLayout = layout;
}

void PageNavigationManager::_transitionFinished(PageWidgetBase *sourcePage, PageWidgetBase *targetPage, bool isShouldDeleteSourcePage)
{
    DLog("---Page transition finished");
    DLog("Page IDs: ") << sourcePage->getPageId() << targetPage->getPageId();
    if(isShouldDeleteSourcePage) {
        DLog("Source page deleted");
        AW_QOBJECT_SAFE_DELETE(sourcePage);
    }
}

void PageNavigationManager::addRootPage(PageWidgetBase *rootPage)
{
    _rootPageStacks.insert(rootPage->getPageId(), new PageNavigationPageStack(rootPage));
}

void PageNavigationManager::switchToRootPageStack(QString rootId, QString pageParam) {
    this->_switchToRootPageStack(rootId, false, pageParam);
}

void PageNavigationManager::_switchToRootPageStack(QString rootId, bool isSwitchOnlyInternallyNotTheUI, QString pageParam)
{
    if(this->_pageTransitionDelegate == NULL) {
        WLog("No page transition delegate found. Cannot operate without it.");
        return;
    }

    if(!_rootPageStacks.contains(rootId))
    {
        WLogS << "This root page is not added. Cannot switch to: " << rootId;
        return;
    }

    if(rootId == _currentRootStack)
    {
        // the specified stack is the current one
#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
        DLog("no switch needed, this is the current root stack: ") << rootId;
#endif
        if(!isSwitchOnlyInternallyNotTheUI) {
            _rootPageStacks.value(_currentRootStack)->getCurrentTopPage()->pageReactivated(pageParam);
        }
        return;
    }


    // activate every page in the target stack
    PageWidgetBase *targetPage = NULL;
    PageNavigationPageStack *pageStack = _rootPageStacks.value(rootId, NULL);
    if( !pageStack->getIsRootInitialized() )
    {
        // this stack is not-yet initialized, so initialize it

#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
        DLogS << "Init root page" << rootId;
#endif

        // root will be activated for the first time
        // initialize root and set as current page
        pageStack->setRootInitialized();

        PageWidgetBase *rootPage = pageStack->getRootPage();
        rootPage->initializePageContent();
        rootPage->pagePushWillHappen();

        //setThisPageAsCurrentContent(rootPage);
        targetPage = rootPage;
    }
    else
    {
        // already initialized, only swapped out because of a previous stack-switch

#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
        DLogS << "Root page initialized. Present it." << rootId;
#endif

        // reactivate every subpages
//        QList<PageWidgetBase *> allPages = pageStack->getAllPages();
//        int allPagesCnt = allPages.size();
//        for(int i = 0; i < allPagesCnt; i++)
//        {
////            allPages[i]->pagePushWillHappen();
//        }

        //setThisPageAsCurrentContent(pageStack->getCurrentTopPage());
        targetPage = pageStack->getCurrentTopPage();
    }

    if(!isSwitchOnlyInternallyNotTheUI)
    {
        PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
        if(!_currentRootStack.isEmpty() && currentNavPageStack != NULL)
        {
            //disconnectPageFromContentArea(currentNavPageStack->getCurrentTopPage());

            PageWidgetBase *sourcePage = currentNavPageStack->getCurrentTopPage();
            sourcePage->pageWillDisappear();
            targetPage->pageWillAppear(pageParam);
            this->_pageTransitionDelegate->performTransition(sourcePage, targetPage, _contentLayout);
        }
        else
        {
            FLAG_FOR_REVIEW_WITH_HINT("Maybe this should happen even if the isSwitchOnlyInternallyNotTheUI is true");
            // there's no visible page
            // push it immediatelly
            targetPage->pageWillAppear(pageParam);
            _contentLayout->addWidget(targetPage);
            targetPage->pageDidAppear();
        }
    }

    _currentRootStack = rootId;
}

void PageNavigationManager::popBackToThisPage(QString pageItemId, QString pageParam)
{
#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
        DLogS << "popBackToThisPage: " << pageItemId;
#endif

    if(this->_pageTransitionDelegate == NULL) {
        WLog("No page transition delegate found. Cannot operate without it.");
        return;
    }

    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no active root stack.");
        return;
    }

    int indexOfThePageInTheStack = currentNavPageStack->indexOfPageInStack(pageItemId);
    if(indexOfThePageInTheStack < 0) {
        WLog("The page is not in the current page stack, cannot pop-back to it.");
        return;
    }


//    QList<PageWidgetBase *> tmpPages = currentNavPageStack->getAllPages();
//    for(int i = 0; i < tmpPages.size(); i++) {
//        DLog("- page id: ") << tmpPages[i]->getPageId();
//    }


    int allPageCnt = currentNavPageStack->getAllPagesCount();
    if(indexOfThePageInTheStack == (allPageCnt - 1) ){
        // the target page is the current page
        DLog("The specified page is the current active page - pageReactivate will be sent.") << indexOfThePageInTheStack << allPageCnt;
        currentNavPageStack->getCurrentTopPage()->pageReactivated(pageParam);
        return;
    }
    if(allPageCnt <= 1) {
        WLog("Cannot pop, it's a root or there's no root page at all.");
        return;
    }

#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
    DLog("Start page activation - deactivation");
#endif

    // save the top-popped page
    PageWidgetBase *currentlyActivePage = currentNavPageStack->popPage();
    currentlyActivePage->pageWillDisappear();
    currentlyActivePage->pagePopWillHappen();

    // and pop the pages until the target
    allPageCnt = currentNavPageStack->getAllPagesCount();
    for(int i = allPageCnt - 1; i > indexOfThePageInTheStack; i--)
    {
        PageWidgetBase *poppedPage = currentNavPageStack->popPage();
        if(poppedPage == NULL) {
            WLog("Root page, cannot be popped.");
            return;
        } else {
            DLog("Delete popped page");
            poppedPage->pageWillDisappear();
            poppedPage->pagePopWillHappen();
            AW_QOBJECT_SAFE_DELETE(poppedPage);
        }
    }


    //disconnectPageFromContentArea(poppedPage);

    PageWidgetBase *newTopPage = currentNavPageStack->getCurrentTopPage();
    newTopPage->pageWillAppear(pageParam);

    // do the transition here
    this->_pageTransitionDelegate->performTransition(currentlyActivePage, newTopPage, _contentLayout, true);

//    AW_QOBJECT_SAFE_DELETE(currentlyActivePage);
}

bool PageNavigationManager::isPageInCurrentlyActiveStack(QString pageItemId)
{
    return this->_isPageInThisStack(pageItemId, _currentRootStack);
}

bool PageNavigationManager::_isPageInThisStack(QString pageId, QString stackId)
{
    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(stackId, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no stack for stackId: ") << stackId;
        return false;
    }

    int indexOfThePageInTheStack = currentNavPageStack->indexOfPageInStack(pageId);

    if(indexOfThePageInTheStack >= 0) {
        // found
        return true;
    }

    // not found
    return false;
}

//bool PageNavigationManager::isPageInCurrentlyActiveStack(QString pageItemId)
//{
//    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
//    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
//    {
//        WLog("There's no active root stack.");
//        return false;
//    }

//    int indexOfThePageInTheStack = currentNavPageStack->indexOfPageInStack(pageItemId);

//    if(indexOfThePageInTheStack >= 0) {
//        // found
//        return true;
//    }

//    // not found
//    return false;
//}

bool PageNavigationManager::isThisPageIsTheCurrentActiveOne(QString pageId)
{
    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no active root stack.");
        return false;
    }

    return (currentNavPageStack->getCurrentTopPage()->getPageId() == pageId);
}

void PageNavigationManager::popBackToThisPageAndSwitchStackIfRequired(QString popToThisPageItemId, QString pageParam)
{
    bool isOnlyPushTheNewPage = false;

    if(this->_pageTransitionDelegate == NULL) {
        WLog("No page transition delegate found. Cannot operate without it.");
        return;
    }

    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no active root stack.");
        return;
    }

    int indexOfThePageToPopInTheStack = currentNavPageStack->indexOfPageInStack(popToThisPageItemId);

    bool isStackChanged = false;
    int allPageCnt = currentNavPageStack->getAllPagesCount();
    if(indexOfThePageToPopInTheStack < 0) {
        QString requiredStackId = this->_getStackIdForPageId(popToThisPageItemId);
        if(requiredStackId.isEmpty()) {
            WLog("The page is not in the current page stack, and cannot find it in any other stacks. Cannot pop-back to it.");
            return;
        }

        this->_switchToRootPageStack(requiredStackId, true, pageParam);
        isStackChanged = true;
    }

    else if(indexOfThePageToPopInTheStack == (allPageCnt - 1) ) {
        // the target page is the current page
        DLog("The target page is the current page. Only pageReactivate will be sent.");
        currentNavPageStack->getCurrentTopPage()->pageReactivated(pageParam);
        return;
    }



    PageWidgetBase *currentlyActivePage = NULL;
    bool isCurrentPageIsRootPage = false;
    if(allPageCnt == 1) {
        isCurrentPageIsRootPage = true;
        // save the root as the currently active one (to page transition)
        currentlyActivePage = currentNavPageStack->getCurrentTopPage();
    }
    else if(allPageCnt > 1 && !isStackChanged) {
        // save the current top page (to page transition)
        currentlyActivePage = currentNavPageStack->popPage();
    }
    else if(allPageCnt > 1) {
        currentlyActivePage = currentNavPageStack->getCurrentTopPage();
    }
    else {
        WLog("Currently there's no page in this stack. Cannot continue, this is some serious bug.");
        return;
    }

    //
    // actualize, if stack change happened
    if(isStackChanged) {
        // store the new stack as current
        currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
        if(currentNavPageStack == NULL) {
            WLog("This should never happen! It's crucial!!");
            return;
        }
        // and also refresh the index of the page
        indexOfThePageToPopInTheStack = currentNavPageStack->indexOfPageInStack(popToThisPageItemId);
        if(indexOfThePageToPopInTheStack < 0) {
            WLog("This should never happen! It's critical!!");
            return;
        }

        int allPageCnt = currentNavPageStack->getAllPagesCount();
        if(indexOfThePageToPopInTheStack == (allPageCnt - 1) ){
            // the target page is the current page, don't pop it just push the new page
            isOnlyPushTheNewPage = true;
        }
    }


    // notify the current page that it will disappear
    currentlyActivePage->pageWillDisappear();
    if(!isCurrentPageIsRootPage && !isStackChanged) {
        // and notify that it will be popped, if it's not a root page
        currentlyActivePage->pagePopWillHappen();
    }

    // and pop the pages until the target
    allPageCnt = currentNavPageStack->getAllPagesCount();
    for(int i = allPageCnt - 1; i > indexOfThePageToPopInTheStack; i--)
    {
        PageWidgetBase *poppedPage = currentNavPageStack->popPage();
        if(poppedPage == NULL) {
            WLog("Root page, cannot be popped. This should never happen, if you see this then it's a serious bug!");
            return;
        } else {
            poppedPage->pageWillDisappear();
            poppedPage->pagePopWillHappen();
            AW_QOBJECT_SAFE_DELETE(poppedPage); // also delete it
        }
    }

    // initialize the new page and notify it that it will be pushed
    PageWidgetBase *newTopPage = currentNavPageStack->getCurrentTopPage();
    newTopPage->pageWillAppear(pageParam);

    bool isDeleteSourcePage = false;
    if(!isCurrentPageIsRootPage && !isStackChanged) {
        // delete the popped page, if it's not a root page and it wasn't a page-stack change
//        AW_QOBJECT_SAFE_DELETE(currentlyActivePage);
        isDeleteSourcePage = true;
    }

    // do the transition here
    this->_pageTransitionDelegate->performTransition(currentlyActivePage, newTopPage, _contentLayout, isDeleteSourcePage);


}

void PageNavigationManager::popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(QString popToThisPageItemId, PageWidgetBase *newPageToPush, QString pageParam)
{
    bool isOnlyPushTheNewPage = false;

    if(this->_pageTransitionDelegate == NULL) {
        WLog("No page transition delegate found. Cannot operate without it.");
        return;
    }

    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no active root stack.");
        return;
    }

    int indexOfThePageToPopInTheStack = currentNavPageStack->indexOfPageInStack(popToThisPageItemId);

    bool isStackChanged = false;
    int allPageCnt = currentNavPageStack->getAllPagesCount();
    if(indexOfThePageToPopInTheStack < 0) {
        QString requiredStackId = this->_getStackIdForPageId(popToThisPageItemId);
        if(requiredStackId.isEmpty()) {
            WLog("The page is not found in any stacks. Won't do anything.");
            return;
        }

        this->_switchToRootPageStack(requiredStackId, true, pageParam);
        isStackChanged = true;
    }

    else {
        if(indexOfThePageToPopInTheStack == (allPageCnt - 1) ){
            // the target page is the current page, don't pop it just push the new page
            this->pushPageToCurrentRootStackAndShow(newPageToPush);
            return;
        }
    }



    PageWidgetBase *currentlyActivePage = NULL;
    bool isCurrentPageIsRootPage = false;
    if(allPageCnt == 1) {
        isCurrentPageIsRootPage = true;
        // save the root as the currently active one (to page transition)
        currentlyActivePage = currentNavPageStack->getCurrentTopPage();
    }
    else if(allPageCnt > 1 && !isStackChanged) {
        // save the current top page (to page transition)
        currentlyActivePage = currentNavPageStack->popPage();
    }
    else if(allPageCnt > 1) {
        currentlyActivePage = currentNavPageStack->getCurrentTopPage();
    }
    else {
        WLog("Currently there's no page in this stack. Cannot continue, this is some serious bug.");
        return;
    }

    //
    // actualize, if stack change happened
    if(isStackChanged) {
        // store the new stack as current
        currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
        if(currentNavPageStack == NULL) {
            WLog("This should never happen! It's crucial!!");
            return;
        }
        // and also refresh the index of the page
        indexOfThePageToPopInTheStack = currentNavPageStack->indexOfPageInStack(popToThisPageItemId);
        if(indexOfThePageToPopInTheStack < 0) {
            WLog("This should never happen! It's crucial!!");
            return;
        }

        int allPageCnt = currentNavPageStack->getAllPagesCount();
        if(indexOfThePageToPopInTheStack == (allPageCnt - 1) ){
            // the target page is the current page, don't pop it just push the new page
            isOnlyPushTheNewPage = true;
        }
    }


    // notify the current page that it will disappear
    currentlyActivePage->pageWillDisappear();
    if(!isCurrentPageIsRootPage && !isStackChanged) {
        // and notify that it will be popped, if it's not a root page
        currentlyActivePage->pagePopWillHappen();
    }

    // and pop the pages until the target
    allPageCnt = currentNavPageStack->getAllPagesCount();
    for(int i = allPageCnt - 1; i > indexOfThePageToPopInTheStack; i--)
    {
        PageWidgetBase *poppedPage = currentNavPageStack->popPage();
        if(poppedPage == NULL) {
            WLog("Root page, cannot be popped. This should never happen, if you see this then it's a serious bug!");
            return;
        } else {
            poppedPage->pageWillDisappear();
            poppedPage->pagePopWillHappen();
            AW_QOBJECT_SAFE_DELETE(poppedPage); // also delete it
        }
    }

    // initialize the new page and notify it that it will be pushed
    newPageToPush->initializePageContent();
    newPageToPush->pagePushWillHappen();
    newPageToPush->pageWillAppear(pageParam);

    // do the transition here
//#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
    DLog("Start page transition");
    DLog("Page ids: ") << currentlyActivePage->getPageId() << newPageToPush->getPageId();
//#endif

    bool isDeleteSourcePage = false;
    if(!isCurrentPageIsRootPage && !isStackChanged) {
        // delete the popped page, if it's not a root page and it wasn't a page-stack change
//        delete currentlyActivePage;
//        currentlyActivePage->deleteLater();
//        AW_QOBJECT_SAFE_DELETE(currentlyActivePage);
        isDeleteSourcePage = true;
    }

    this->_pageTransitionDelegate->performTransition(currentlyActivePage, newPageToPush, _contentLayout, isDeleteSourcePage);
//#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
    DLog("Page transition started");
//#endif

    // and store the pushed page in the stack
    currentNavPageStack->pushPage(newPageToPush);

    DLog("- end of method");
}

bool PageNavigationManager::isThisRootStackIsTheActiveOne(QString rootId)
{
    return (_currentRootStack == rootId);
}

void PageNavigationManager::pushPageToCurrentRootStackAndShow(PageWidgetBase* page, QString pageParam) {

    if(this->_pageTransitionDelegate == NULL) {
        WLog("No page transition delegate found. Cannot operate without it.");
        return;
    }

    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no active root stack. Cannot push into it.");
        return;
    }


    PageWidgetBase *currTopPage = currentNavPageStack->getCurrentTopPage();
    currTopPage->pageWillDisappear();

    page->initializePageContent();
    page->pagePushWillHappen();
    page->pageWillAppear(pageParam);

    // do the transition here
    this->_pageTransitionDelegate->performTransition(currTopPage, page, _contentLayout);

    //contentLayout->insertWidget(indexOfCurrTopPage, page, 0, Qt::AlignTop | Qt::AlignCenter);


    //currTopPage->setParent(NULL);
//#endif

    //contentWidgets.append(widget);
    currentNavPageStack->pushPage(page);
#if 0
    contentWidgets.last()->initializePageContent();
    contentWidgets.last()->pagePushWillHappen();
    contentWidgets.last()->pageWillAppear(pageParam);
    contentLayout->addWidget(contentWidgets.last(), 0, Qt::AlignTop | Qt::AlignCenter);
#else

    //setThisPageAsCurrentContent(page);
#endif

//#ifdef ENABLE_PAGE_NAVIGATION_LOGGING
    DLog(QString("Page (%1) pushed to stack (%2)").arg(page->getPageId()).arg(_currentRootStack));
//#endif

}

void PageNavigationManager::popPageFromCurrentRootStackAndRemove()
{
    if(this->_pageTransitionDelegate == NULL) {
        WLog("No page transition delegate found. Cannot operate without it.");
        return;
    }

    PageNavigationPageStack *currentNavPageStack = _rootPageStacks.value(_currentRootStack, NULL);
    if(currentNavPageStack == NULL || _currentRootStack.isEmpty())
    {
        WLog("There's no active root stack. Cannot push into it.");
        return;
    }

    PageWidgetBase *poppedPage = currentNavPageStack->popPage();
    if(poppedPage == NULL)
    {
        WLog("Root item cannot be popped.");
        return;
    }

    poppedPage->pageWillDisappear();
    poppedPage->pagePopWillHappen();
    //disconnectPageFromContentArea(poppedPage);

    PageWidgetBase *newTopPage = currentNavPageStack->getCurrentTopPage();
    newTopPage->pageWillAppear(QString());

    // do the transition here
    this->_pageTransitionDelegate->performTransition(poppedPage, newTopPage, _contentLayout, true);

//    poppedPage->setParent(NULL);
//    AW_QOBJECT_SAFE_DELETE(poppedPage);
}

QString PageNavigationManager::_getStackIdForPageId(QString pageId) {
    bool isFound = false;
    QString _retStackId;
    QHash<QString,PageNavigationPageStack *>::const_iterator i = _rootPageStacks.constBegin();
    while (i != _rootPageStacks.constEnd() && !isFound) {
        if( this->_isPageInThisStack(pageId, i.key()) ) {
            _retStackId = i.key();
            isFound = true;
        }

        ++i;
    }

    return _retStackId;
}

#if 0
void PageNavigationManager::disconnectPageFromContentArea(BrowserPageWidgetInterface *page)
{
    page->pageWillDisappear();
    contentLayout->removeWidget(page);
    page->setParent(NULL);
}
#endif

#if 0
BrowserWidgetHolder *PageNavigationManager::getCurrentPage()
{
    if(contentWidgets.isEmpty())
    {
        return NULL;
    }

    return contentWidgets.last();
}
#endif
