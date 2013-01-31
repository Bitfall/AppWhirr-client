#ifndef CHECKRESULTLISTWINDOW_H
#define CHECKRESULTLISTWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "AppSpec/Database/websitelistdatabaseitemmodel.h"
#include "compareitemswindow.h"

QT_BEGIN_NAMESPACE
class QTableView;
class QStandardItemModel;
QT_END_NAMESPACE

class CheckResultListWindow : public QMainWindow, public CompareItemsWindow::CompareItemsWindowActionDelegate
{
    Q_OBJECT

public:

    class CheckResultListWindowDelegate {
    public:
        // contains the result items
        //  after every required item is checked it will have the final version of the contents
        //      e.g. if the user specified "ignore" for an item it will contain the original content
        //      and if the user specified "update" for an item then it will contain the new content
        virtual void saveFinalResultItems(QList<WebsiteListDatabaseItemModel> resultItems) = 0;

        // [%]
        // temporarily won't open the urls but it will save them to file
        // because somehow I couldn't manage to open multiple urls at the same time - at least 1 of them doesn't open correctly
        virtual void saveUpdatedAppLinks(QList<QString> urlList) = 0;
    };

    enum ActionStatus {
        AS_NoAction = 0,
        AS_Same,
        AS_FirstCheck,
        AS_KeepOriginalContent,
        AS_UseNewContentButNotAnUpdate,
        AS_UseNewContentAndMarkUpdate
    };

private:
    class CheckResultListInternalItem {
    public:
        explicit CheckResultListInternalItem() {}

        explicit CheckResultListInternalItem(WebsiteListDatabaseItemModel originalItem, WebsiteListDatabaseItemModel checkResultItem, /*WebsiteListDatabaseItemModel finalResultItem,*/ ActionStatus relatedActionStatus) : _originalItem(originalItem), _checkResultItem(checkResultItem), /*_finalResultItem(finalResultItem),*/ _relatedActionStatus(relatedActionStatus)
        {}

        WebsiteListDatabaseItemModel getOriginalItem() const { return _originalItem; }
        WebsiteListDatabaseItemModel getCheckResultItem() const { return _checkResultItem; }
        WebsiteListDatabaseItemModel getFinalResultItem() const { return _finalResultItem; }
        ActionStatus getRelatedActionStatus() const { return _relatedActionStatus; }

        void _setFinalResultItem(WebsiteListDatabaseItemModel item) { _finalResultItem = item; }
        void setRelatedActionStatus(ActionStatus value) { _relatedActionStatus = value; }

    private:
        WebsiteListDatabaseItemModel _originalItem;
        WebsiteListDatabaseItemModel _checkResultItem;
        WebsiteListDatabaseItemModel _finalResultItem;

        ActionStatus _relatedActionStatus;
    };

public:
    explicit CheckResultListWindow(CheckResultListWindowDelegate *delegate, QWidget *parent = 0);

    bool setCheckItems(QList<WebsiteListDatabaseItemModel> originalItems, QList<WebsiteListDatabaseItemModel> checkResultItems);

    //
    // delegate implementation methods
    void keepTheOriginalContentActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem);
    void useTheNewContentButNotAnUpdateActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem);
    void useTheNewContentAndUpdateActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem);

protected:
    void closeEvent(QCloseEvent *e);

private Q_SLOTS:
//    void saveResultButtonPressed();
//    void openUpdatedLinksButtonPressed();

    void itemClickedInResultListTableView(QModelIndex index);

    void addNewItemButtonPressed();
    void _addThisItem(WebsiteListDatabaseItemModel newItem);
    void _itemEdited(WebsiteListDatabaseItemModel editedItem);

private:
    void doCompareOfItemsAndBuildAndPresentFianlItemResultInTableView();
    QString getPresentableStatusTextByActionStatus(ActionStatus actionStatus);

    void refreshViewByFinalResultItems();
    void replaceItemAndRefreshStatusInFinalResultsAndInView(WebsiteListDatabaseItemModel newItem, int indexToReplace, ActionStatus newActionStatus);
    void replaceItemAndRefreshInFinalResultsAndInView(WebsiteListDatabaseItemModel newItem, int indexToReplace);

    void _addOrSetItemToResultListForView(WebsiteListDatabaseItemModel newItem, ActionStatus status, int indexToInsertInto);
    void _removeThisItemCompletelyByPresentedIndex(int presentedItemIndex);

    int __getInternalItemIndexByPresentedItemIndex(int presentedItemIndex);
    bool __getInternalItemByPresentedItemIndex(int presentedItemIndex, CheckResultListWindow::CheckResultListInternalItem &outInternalItem);

    QList<QString> getUpdatedLinks();

private:
    CheckResultListWindowDelegate *_delegate;

//    QList<WebsiteListDatabaseItemModel> _originalItems;
//    QList<WebsiteListDatabaseItemModel> _checkResultItems;
//    QList<WebsiteListDatabaseItemModel> _finalResultItems;

//    QList<ActionStatus> _finalResultItemsActionStatuses;

    QList<CheckResultListInternalItem> _internalItems;

    QList<int> _presentedItemIndicesToInternalItemIndices;
    QStandardItemModel *_finalResultModelForView;

    int _currSelectedPresentedViewItemRowIndex;
    QTableView *_comparedResultListTableView;
};

#endif // CHECKRESULTLISTWINDOW_H
