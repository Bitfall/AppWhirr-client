#ifndef PAGENAVIGATIONMANAGER_H
#define PAGENAVIGATIONMANAGER_H

#include <QObject>
#include <QList>
#include <QWidget>
#include <QHash>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

#include "Common/Page/pagewidgetbase.h"
#include "Delegate/pagetransitionmanagerdelegateinterface.h"

class PageNavigationPageStack;

/*! Manages pages and page-navigation actions.

    @remark Important:
    This manger should NOT change the layout -> the transition-delegate have to!

    Both \a contentLayout and \a pageTransitionDelegate have to be set before use.
*/
class PageNavigationManager : public QObject
{
    Q_OBJECT

public:
    /*!
        @param pageTransitionDelegate : the page-transition-delegate. Will be used to perform the transitions between pages with or without animation.
        @param pageContentHolderLayout : the content layout which will contain the current (presented) page.
    */
    explicit PageNavigationManager(PageTransitionManagerDelegateInterface *pageTransitionDelegate, QVBoxLayout *pageContentHolderLayout, QObject *parent = 0);
    ~PageNavigationManager();

    // ------------------------------
    // --- main interface methods ---

    /*! Adds a root page

        A root page won't have any parent page. It cannot be popped. Also it will create a new page-stack as well, so child-pages can be pushed under it.

        @remark PageId:
        The given \a rootPage 's pageId will be used as the ID of it's stack. For example: when you call the \a switchToRootPageStack() method the \a rootId parameter (which identifies the stack) is the root page's pageId

        @note Initialization:
        Will only add the page, won't initialize or show it - call switchToRootPageStack to activate a root page (this allows lower memory usage in case the page is not constructed in it's constructor but in initializePageContent(), which will be called when it first activated)
    */
    void addRootPage(PageWidgetBase *rootPage);

    /*! Switch from the current stack to the defined one.

        Won't remove or delete the current page, only calls it's \a pageWillDisappear() method.

        @param rootId : the page-stack's root page's pageId
        @param pageParam : the parameter will be passed to the appearing page's \a pageWillAppear() method
    */
    void switchToRootPageStack(QString rootId, QString pageParam = QString());

    /*! Switches directly to the page, drops every sub-pages of it.

        The page have to be in the currently active root-page-stack. If it's not then it will do nothing.

        @param pageItemId : the pageId of the page you want to pop-back at and show
        @param pageParam : the parameter will be passed to the appearing page's \a pageWillAppear() method
    */
    void popBackToThisPage(QString pageItemId, QString pageParam = QString());

    /*! Push to the current root-stack and start the page-transition from the current page to this one.

        @param pageParam : the parameter will be passed to the appearing page's \a pageWillAppear() method.
    */
    void pushPageToCurrentRootStackAndShow(PageWidgetBase *page, QString pageParam = QString());

    /*! Pops the current page if it's not a root page.

        It may also delete the page will be popped (free up it's memory).

        @param pageParam : is the parameter will be passed to the appearing page's pageWillAppear() method
    */
    void popPageFromCurrentRootStackAndRemove();

    /*! A flexible version of \a pushPageToCurrentRootStackAndShow()

        The \a pushPageToCurrentRootStackAndShow() method will push the given page to the current-root-stack. With this method you can push and show a page into another page-stack.\n
        You can safely use this even if the specified popToThisPageItemId is the current page - in this case it will only push the new page, won't pop the current page
        If the specified popToThisPageItemId is not found in the current root-stack it will search for in other stacks and switch to the stack it founds the page in.

        @remark Important:
        If no page found with pageId = \a popToThisPageItemId then it won't do anything. Not even the push.

        @param pageParam : is the parameter will be passed to the appearing page's pageWillAppear() method
    */
    void popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(QString popToThisPageItemId, PageWidgetBase *newPageToPush, QString pageParam = QString());

    /*! Similar to \a popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired() method but it won't push a new page.

        For restrictions @see popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired()

        @param pageParam : is the parameter will be passed to the appearing page's pageWillAppear() method
    */
    void popBackToThisPageAndSwitchStackIfRequired(QString popToThisPageItemId, QString pageParam = QString());
    //BrowserWidgetHolder *getCurrentPage();



    // ----------------------------------
    // --- utility and helper methods ---
    bool isPageInCurrentlyActiveStack(QString pageItemId);
    bool isThisRootStackIsTheActiveOne(QString rootId);
    bool isThisPageIsTheCurrentActiveOne(QString pageId);

    /*! Replaces the current page-transition-delegate with the new one.

        @remark Important:
        The current page-transition-delegate will be deleted.
    */
    void _setPageTransitionDelegate(PageTransitionManagerDelegateInterface *pageTransitionDelegate);

    /*! Replaces the current page-content-layou with the new one.
    */
    void _setContentLayout(QVBoxLayout* layout);

private Q_SLOTS:
    void _transitionFinished(PageWidgetBase *sourcePage, PageWidgetBase *targetPage, bool isShouldDeleteSourcePage);

private:
    QString _getStackIdForPageId(QString pageId);
    bool _isPageInThisStack(QString pageId, QString stackId);
    //
    // If isSwitchOnlyInternallyNotTheUI is true then it won't start a page-transition animation.
    //  It's purpose is to allow to combine other methods with stack-switch, if required.
    //  ! If isSwitchOnlyInternallyNotTheUI is true then the source page's pageWillDisappear, pop, ... methods are also won't be called.
    //
    // pageParam is the parameter will be passed to the appearing page's pageWillAppear() method
    void _switchToRootPageStack(QString rootId, bool isSwitchOnlyInternallyNotTheUI, QString pageParam);

private:
    //QList<BrowserPageWidgetInterface *> contentWidgets;
    QHash< QString, PageNavigationPageStack * > _rootPageStacks; // this is the always up-to-date container, perform switch / add / remove operations with / through this
    QVBoxLayout *_contentLayout;
    QString _currentRootStack;

    PageTransitionManagerDelegateInterface *_pageTransitionDelegate;
};

#endif // PAGENAVIGATIONMANAGER_H
