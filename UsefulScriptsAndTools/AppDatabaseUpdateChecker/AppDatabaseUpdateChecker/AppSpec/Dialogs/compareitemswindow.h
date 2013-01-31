#ifndef COMPAREITEMSWINDOW_H
#define COMPAREITEMSWINDOW_H

#include <QMainWindow>

#include "AppSpec/Database/websitelistdatabaseitemmodel.h"

QT_BEGIN_NAMESPACE
class QWebView;
class QTextEdit;
QT_END_NAMESPACE

class CompareItemsWindow : public QMainWindow
{
    Q_OBJECT
public:
    class CompareItemsWindowActionDelegate {
    public:
        virtual void keepTheOriginalContentActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem) = 0;
        virtual void useTheNewContentButNotAnUpdateActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem) = 0;
        virtual void useTheNewContentAndUpdateActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem) = 0;
    };

public:
    explicit CompareItemsWindow(CompareItemsWindowActionDelegate *actionDelegate, WebsiteListDatabaseItemModel originalItem, WebsiteListDatabaseItemModel newItem, QWidget *parent = 0);

private Q_SLOTS:
    void originalLinkContentPresenterWebViewScroll(int dx, int dy, const QRect& scrollViewRect);
    void newLinkContentPresenterWebViewScroll(int dx, int dy, const QRect& scrollViewRect);

    void keepTheOriginalContentButtonPressed();
    void useTheNewContentButNotAnUpdateButtonPressed();
    void useTheNewContentAndUpdateButtonPressed();

private:
    QWebView *_originalLinkContentPresenterWebView;
    QWebView *_newLinkContentPresenterWebView;

    CompareItemsWindowActionDelegate *_actionDelegate;

    WebsiteListDatabaseItemModel _originalItem;
    WebsiteListDatabaseItemModel _newItem;

    QTextEdit *_checkSectionBeginTextEdit;
    QTextEdit *_checkSectionEndTextEdit;

    QTextEdit *_noteTextEdit;
};

#endif // COMPAREITEMSWINDOW_H
