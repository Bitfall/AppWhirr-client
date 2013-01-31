#ifndef PAGEWIDGETBASE_H
#define PAGEWIDGETBASE_H

#include <QWidget>

/*! Base class for pages.

    The page (widget) itself has to be able to present it's content in the size the page-holder requires.\n
    It means: the reference size is the page-holder's content size, inwhich the page will be inserted. And in most cases this size will be forced to be used by the page.\n
    So handle the page az a dynamic-sized content holder object/widget, which can be expanded as the system requires, and dynamically position it's content regarding the size (a simple layout is perfect for this).\n
    @remark Warning:
    ! \b Don't set fixed-size, or minimum/maximum size for a page! It always has SizePolicy = Expanding in both width and height!
*/
class PageWidgetBase : public QWidget
{
public:
    explicit PageWidgetBase(QString pageId, QWidget *parent = 0);

    virtual ~PageWidgetBase();

    /*! Initialize the content here

        This method will be called only once, when the page have to initialize it's content (and related data).

        @remark Page transition:
        The page has to fully initialize it's initial "outfit" / layout here because after this method some page transition can happen and that may use the page's state / look rigth after this method
    */
    virtual void initializePageContent() = 0;

    /*! This method will be called only when the page will be pushed into the navigation controller (won't be called, when it will appear via pop)

        @note Menu:
        You should initialize and add menu items here
    */
    virtual void pagePushWillHappen() = 0;

    /*! This method will be called when the page is popped

        @note Menu:
        Ideal place to remove the custom menu-items added by the page

        @note Root page:
        Root pages will never get this method be called
    */
    virtual void pagePopWillHappen() = 0;

    /*! This method will be called every time before the page appear, before any transition-animation.

        It can get a \a param string which can contain some additional informations for pages have different "modes". Like: show X page with items Y. It's totally optional.

        @note Page transition:
        After this some page-transition animation can happen.
        The page is not ready for interaction yet.
    */
    virtual void pageWillAppear(QString param) = 0;

    /*! This method will be called every time the page is fully activated.

        In this method you can be sure that the page is totally visible and interaction can be performed, for example some appearing animations / content actualization animations.\n
        Generally: when this method is called the page is fully initialized and you can perform any operation you want. User interaction is only allowed after this method is called.
    */
    virtual void pageDidAppear() = 0;

    /*! The page is the current page, but it is reactivated with the given param.

        The \a param is the same optional param as in the \a pageWillAppear() method
    */
    virtual void pageReactivated(QString param) {}

    /*! This method will be called every time the page will disappear.

        You can perform some operations here, but you cannot stop the page to disappear. It will happen whether you want it or not.

        @remark Important:
        After this method called the page may be deleted (memory freed up).
    */
    virtual void pageWillDisappear() = 0;

    /*! Unique page id

        @remark Unique:
        This method have to return a unique page-id. This ID will identify this page in the \a PageNavigationManager

        @remark Important:
        Don't change the id after you added the page to the navigation manager, it may cache it!
    */
    QString getPageId() const;

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    void setPageId(QString id);

private:
    QString _pageId;
};

#endif // PAGEWIDGETBASE_H
