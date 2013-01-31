#include "checkresultlistwindow.h"

#include <QTableView>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QMessageBox>
#include <QMetaType>

#include <QCloseEvent>

#include "AppSpec/Database/websitelistdatabasemanager.h"

#include "addnewitemdialog.h"

#include "Common/Helpers/qstringhelper.h"

CheckResultListWindow::CheckResultListWindow(CheckResultListWindowDelegate *delegate, QWidget *parent) :
    QMainWindow(parent),
    _delegate(delegate)
{
    this->resize(800, 300);

//    this->_originalItems = originalItems;
//    this->_checkResultItems = checkResultItems;

    QWidget *mainCentralWidget = new QWidget;
    this->setCentralWidget(mainCentralWidget);

    QGridLayout *mainLayout = new QGridLayout;
    mainCentralWidget->setLayout(mainLayout);

    int gridLayoutRowIndex = 0;

    QPushButton *addNewItemButton = new QPushButton(tr("Add new item"));
    connect(addNewItemButton, SIGNAL(clicked()), this, SLOT(addNewItemButtonPressed()));
    mainLayout->addWidget(addNewItemButton, gridLayoutRowIndex, 0, 1, -1);

    // -- new row
    gridLayoutRowIndex++;

    _comparedResultListTableView = new QTableView;
    connect(_comparedResultListTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClickedInResultListTableView(QModelIndex)));
    mainLayout->addWidget(_comparedResultListTableView, gridLayoutRowIndex, 0, 1, -1);

    // -- new row
//    gridLayoutRowIndex++;

//    QPushButton *saveResultsButton = new QPushButton;
//    saveResultsButton->setText(tr("Save results to file"));
//    connect(saveResultsButton, SIGNAL(clicked()), this, SLOT(saveResultButtonPressed()));
//    mainLayout->addWidget(saveResultsButton, gridLayoutRowIndex, 0);

//    QPushButton *openUpdatedLinksButton = new QPushButton;
//    openUpdatedLinksButton->setText(tr("Save links marked as Updated to file"));
//    connect(openUpdatedLinksButton, SIGNAL(clicked()), this, SLOT(openUpdatedLinksButtonPressed()));
//    mainLayout->addWidget(openUpdatedLinksButton, gridLayoutRowIndex, 1);


    qRegisterMetaType<WebsiteListDatabaseItemModel>("WebsiteListDatabaseItemModel");
}

bool CheckResultListWindow::setCheckItems(QList<WebsiteListDatabaseItemModel> originalItems, QList<WebsiteListDatabaseItemModel> checkResultItems)
{
    if(originalItems.size() != checkResultItems.size()) {
        WLog("Original and check-result list size is NOT EQUAL.");
        return false;
    }

    //
    // size checked, build the internal item list
    for(int i = 0; i < originalItems.size(); i++)
    {
        this->_internalItems.append(CheckResultListWindow::CheckResultListInternalItem(originalItems[i], checkResultItems[i], AS_NoAction));
    }

    this->doCompareOfItemsAndBuildAndPresentFianlItemResultInTableView();

    return true;
}

void CheckResultListWindow::addNewItemButtonPressed()
{
    AddOrEditItemDialog *addNewItemWindow = new AddOrEditItemDialog(this);
    connect(addNewItemWindow, SIGNAL(newItemAdded(WebsiteListDatabaseItemModel)), this, SLOT(_addThisItem(WebsiteListDatabaseItemModel)));
    addNewItemWindow->showNormal();
}

void CheckResultListWindow::_removeThisItemCompletelyByPresentedIndex(int presentedItemIndex)
{
    DLog("Presented item index: ") << presentedItemIndex;

    int dialogRes = QMessageBox::warning(this, tr("Confirm remove"), tr("Want to remove?"), tr("Remove"), tr("Cancel"));

    if(dialogRes == 0) {
        int relatedInternalItemIndex = this->__getInternalItemIndexByPresentedItemIndex(presentedItemIndex);

        if(relatedInternalItemIndex < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Invalid internal item index. Cannot remove."));
            return;
        }

        _finalResultModelForView->removeRow(presentedItemIndex);


        this->_internalItems.removeAt(relatedInternalItemIndex);
        _presentedItemIndicesToInternalItemIndices.removeAt(presentedItemIndex);

        //
        // also have to decrement every index-linking after the deleted one to link correctly
        for(int i = 0; i < _presentedItemIndicesToInternalItemIndices.size(); i++) {
            if(_presentedItemIndicesToInternalItemIndices[i] > relatedInternalItemIndex) {
                DLog("Decrement this: ") << _presentedItemIndicesToInternalItemIndices[i];
                _presentedItemIndicesToInternalItemIndices[i] = _presentedItemIndicesToInternalItemIndices[i] - 1;
            }
        }
    }
}

void CheckResultListWindow::doCompareOfItemsAndBuildAndPresentFianlItemResultInTableView()
{
    DLog("-- start doCompareOfItemsAndBuildAndPresentFianlItemResultInTableView");

    int itemCnt = _internalItems.size();
//    if(itemCnt != this->_checkResultItems.size()) {
//        QMessageBox::warning(this, tr("Error"), tr("Original and checked list item count is not the same. Should be..."));
//        return;
//    }

    //
    // we will sort the items -> the items require some action are moved to the top, and the ones doesn't require moved to the bottom

    for(int i = 0; i < itemCnt; i++) {
        WebsiteListDatabaseItemModel currModel;
        ActionStatus currStatus = AS_NoAction;

        //
        // check whether it's the first check
        if(_internalItems[i].getOriginalItem().getContent().isEmpty() && !_internalItems[i].getCheckResultItem().getContent().isEmpty()) {
            currStatus = AS_FirstCheck;
            currModel = _internalItems[i].getCheckResultItem();
            DLog("First check for app: ") << _internalItems[i].getOriginalItem().getAppName();
        }
        else
        {
            // not the first check

            bool isCheckDoneInSection = false;
            //
            // get the check-section if specified
            if(!_internalItems[i].getOriginalItem().getCheckSectionBegin().isEmpty() && !_internalItems[i].getOriginalItem().getCheckSectionEnd().isEmpty())
            {
                QString checkSectionBeginIndicator = _internalItems[i].getOriginalItem().getCheckSectionBegin();
                QString checkSectionEndIndicator = _internalItems[i].getOriginalItem().getCheckSectionEnd();

                // check-section specified -> search for it
                QString originalCheckSectionContent;
                QString checkResultCheckSectionContent;

                QString originalContent = _internalItems[i].getOriginalItem().getContent();
                QString checkResultContent = _internalItems[i].getCheckResultItem().getContent();


                if(QStringHelper::getFirstSectionByBeginEndIndicators(originalContent, checkSectionBeginIndicator, checkSectionEndIndicator, originalCheckSectionContent) && QStringHelper::getFirstSectionByBeginEndIndicators(checkResultContent, checkSectionBeginIndicator, checkSectionEndIndicator, checkResultCheckSectionContent))
                {
                    // section found in both - compare
                    originalCheckSectionContent = originalCheckSectionContent.simplified();
                    checkResultCheckSectionContent = checkResultCheckSectionContent.simplified();

                    DLog("Section found.");
                    DLog("Section in original: ") << originalCheckSectionContent;
                    DLog("Section in check-result: ") << checkResultCheckSectionContent;

                    isCheckDoneInSection = true;


                    if(originalCheckSectionContent == checkResultCheckSectionContent) {
                        DLog("Section MATCH.");
                        currStatus = AS_Same;
                        currModel = _internalItems[i].getCheckResultItem();
                    }
                    else {
                        DLog("Sections does NOT match.");
                        currModel = _internalItems[i].getCheckResultItem();
                    }
                }

                if(!isCheckDoneInSection) {
                    WLog("Section defined but failed to compare.");
                    QMessageBox::warning(this, tr("Warning"), tr("Section defined for app (%1) but cannot find the specified section in the original or in the new content.").arg(_internalItems[i].getOriginalItem().getAppName()));
                }
            }

            if(!isCheckDoneInSection)
            {
                // no check section defined or failed - compare the whole content
                DLog("Compare whole contents: ") << _internalItems[i].getOriginalItem().getAppName();

                // compare
                if(_internalItems[i].getOriginalItem().getContent() == _internalItems[i].getCheckResultItem().getContent()) {
                    currStatus = AS_Same;
                    currModel = _internalItems[i].getCheckResultItem();
                }
                else {
                    // not same and not the first content check - compare is required and action have to be selected

                    // pick the new version's item-model, but will be compared
                    currModel = _internalItems[i].getCheckResultItem();
                }
            }
        }

        // add the curr-item-model to the final-result list and it's status
        _internalItems[i]._setFinalResultItem(currModel);
        _internalItems[i].setRelatedActionStatus(currStatus);
        if(currStatus == AS_NoAction) {
            // action required - present at the top
//            this->_finalResultItems.prepend(currModel);
//            this->_finalResultItemsActionStatuses.prepend(currStatus);
            this->_presentedItemIndicesToInternalItemIndices.prepend(i);
        }
        else {
            // no action required - append to the end of the list
//            this->_finalResultItems.append(currModel);
//            this->_finalResultItemsActionStatuses.append(currStatus);
            this->_presentedItemIndicesToInternalItemIndices.append(i);
        }
    }

    this->refreshViewByFinalResultItems();
}

void CheckResultListWindow::_addOrSetItemToResultListForView(WebsiteListDatabaseItemModel newItem, ActionStatus status, int indexToInsertInto)
{
    // and define it's view-model
    QStandardItem *item = new QStandardItem(newItem.getAppName());
    _finalResultModelForView->setItem(indexToInsertInto, 0, item);

    item = new QStandardItem(newItem.getUrl());
    _finalResultModelForView->setItem(indexToInsertInto, 1, item);

    item = new QStandardItem(newItem.getTimestampAsText());
    _finalResultModelForView->setItem(indexToInsertInto, 2, item);

    item = new QStandardItem(this->getPresentableStatusTextByActionStatus(status));
    _finalResultModelForView->setItem(indexToInsertInto, 3, item);

    item = new QStandardItem(tr(">>Do-Compare<<"));
    _finalResultModelForView->setItem(indexToInsertInto, 4, item);

    item = new QStandardItem(tr("!! Remove !!"));
    _finalResultModelForView->setItem(indexToInsertInto, 5, item);

    item = new QStandardItem(tr("[Edit]"));
    _finalResultModelForView->setItem(indexToInsertInto, 6, item);
}

void CheckResultListWindow::refreshViewByFinalResultItems()
{
    int itemCnt = _presentedItemIndicesToInternalItemIndices.size();

    _finalResultModelForView = new QStandardItemModel(itemCnt, 5);

    _finalResultModelForView->setHorizontalHeaderLabels(QStringList() << tr("App name") << tr("Check url") << tr("Check timestamp") << tr("Action status") << tr("Compare"));

    for(int i = 0; i < itemCnt; i++) {
        CheckResultListWindow::CheckResultListInternalItem relatedInternalItem;
        if(!this->__getInternalItemByPresentedItemIndex(i, relatedInternalItem)) {
            WLog("Cannot get the related internal item. Will be skipped.");
            continue;
        }

        WebsiteListDatabaseItemModel currModel = relatedInternalItem.getFinalResultItem();
        ActionStatus currStatus = relatedInternalItem.getRelatedActionStatus();

        this->_addOrSetItemToResultListForView(currModel, currStatus, i);
    }

    this->_comparedResultListTableView->setModel(_finalResultModelForView);
}


int CheckResultListWindow::__getInternalItemIndexByPresentedItemIndex(int presentedItemIndex)
{
    if(presentedItemIndex >= this->_presentedItemIndicesToInternalItemIndices.size()) {
        return -1;
    }

    int internalItemIdx = _presentedItemIndicesToInternalItemIndices[presentedItemIndex];
    if(internalItemIdx >= _internalItems.size()) {
        return -1;
    }

    return internalItemIdx;
}

bool CheckResultListWindow::__getInternalItemByPresentedItemIndex(int presentedItemIndex, CheckResultListWindow::CheckResultListInternalItem &outInternalItem)
{
    int internalItemIdx = this->__getInternalItemIndexByPresentedItemIndex(presentedItemIndex);
    if(internalItemIdx < 0) {
        return false;
    }

    outInternalItem = _internalItems[internalItemIdx];
    return true;
}

void CheckResultListWindow::itemClickedInResultListTableView(QModelIndex index)
{
    _currSelectedPresentedViewItemRowIndex = index.row();

    if(index.column() == 4) {
        // do-compare clicked
//        int originalIndexOfThisItem = this->_presentedItemIndexToInternalItemIndex[_currSelectedItemRowIndex];
        CheckResultListWindow::CheckResultListInternalItem relatedInternalItem;
        if(!this->__getInternalItemByPresentedItemIndex(_currSelectedPresentedViewItemRowIndex, relatedInternalItem)) {
            WLog("Cannot get the related internal item for presented index: ") << _currSelectedPresentedViewItemRowIndex;
            return;
        }

        CompareItemsWindow *compareItemsWindow = new CompareItemsWindow(this, relatedInternalItem.getOriginalItem(), relatedInternalItem.getCheckResultItem(), this);
        compareItemsWindow->showNormal();
    }
    else if(index.column() == 5) {
        // remove clicked

//        int originalIndexOfThisItem = this->_finalResultItemsOriginalIndices[_currSelectedItemRowIndex];
//        CompareItemsWindow *compareItemsWindow = new CompareItemsWindow(this, this->_originalItems[originalIndexOfThisItem], this->_checkResultItems[originalIndexOfThisItem], this);
//        compareItemsWindow->showNormal();

        this->_removeThisItemCompletelyByPresentedIndex(_currSelectedPresentedViewItemRowIndex);
    }
    else if(index.column() == 6) {
        // edit
        CheckResultListWindow::CheckResultListInternalItem relatedInternalItem;
        if(!this->__getInternalItemByPresentedItemIndex(_currSelectedPresentedViewItemRowIndex, relatedInternalItem)) {
            QMessageBox::warning(this, tr("Error"), tr("Invalid internal item index"));
            return;
        }

        // test whether have to be selected
        if(relatedInternalItem.getRelatedActionStatus() == AS_NoAction) {
            QMessageBox::information(this, tr("No action selected."), tr("No action selected for this item - you have to select and action before editing."));
            return;
        }

        AddOrEditItemDialog *addOrEditItemWindow = new AddOrEditItemDialog(relatedInternalItem.getFinalResultItem());
        connect(addOrEditItemWindow, SIGNAL(itemEdited(WebsiteListDatabaseItemModel)), this, SLOT(_itemEdited(WebsiteListDatabaseItemModel)));
        addOrEditItemWindow->showNormal();
    }
}



void CheckResultListWindow::_addThisItem(WebsiteListDatabaseItemModel newItem)
{
    int newItemIndex = _internalItems.size();

    CheckResultListWindow::CheckResultListInternalItem newInternalModel(newItem, newItem, AS_FirstCheck);
    newInternalModel._setFinalResultItem(newItem);
    _internalItems.append(newInternalModel);
    this->_presentedItemIndicesToInternalItemIndices.append(newItemIndex);

//    QStandardItem *modelItem = new QStandardItem(this->getPresentableStatusTextByActionStatus(AS_FirstCheck));
//    _finalResultModelForView->setItem(this->_currSelectedItemRowIndex, 3, modelItem);

    this->_addOrSetItemToResultListForView(newItem, AS_FirstCheck, newItemIndex);
}

void CheckResultListWindow::_itemEdited(WebsiteListDatabaseItemModel editedItem)
{
    // refresh original and check-result as well
    this->replaceItemAndRefreshInFinalResultsAndInView(editedItem, this->_currSelectedPresentedViewItemRowIndex);
}

void CheckResultListWindow::keepTheOriginalContentActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem) {
    this->replaceItemAndRefreshStatusInFinalResultsAndInView(relatedSelectedItem, this->_currSelectedPresentedViewItemRowIndex, AS_KeepOriginalContent);
}

void CheckResultListWindow::useTheNewContentButNotAnUpdateActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem) {
    this->replaceItemAndRefreshStatusInFinalResultsAndInView(relatedSelectedItem, this->_currSelectedPresentedViewItemRowIndex, AS_UseNewContentButNotAnUpdate);
}

void CheckResultListWindow::useTheNewContentAndUpdateActionSelected(WebsiteListDatabaseItemModel relatedSelectedItem)
{
    this->replaceItemAndRefreshStatusInFinalResultsAndInView(relatedSelectedItem, this->_currSelectedPresentedViewItemRowIndex, AS_UseNewContentAndMarkUpdate);
}

void CheckResultListWindow::replaceItemAndRefreshInFinalResultsAndInView(WebsiteListDatabaseItemModel newItem, int indexToReplace)
{
    int relatedInternalItemIdx = this->__getInternalItemIndexByPresentedItemIndex(indexToReplace);
    if(relatedInternalItemIdx < 0) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid internal item index"));
        return;
    }
    this->_internalItems[relatedInternalItemIdx]._setFinalResultItem(newItem);

//    QStandardItem *modelItem = new QStandardItem(this->getPresentableStatusTextByActionStatus(newActionStatus));
//    _finalResultModelForView->setItem(indexToReplace, 3, modelItem);
    this->_addOrSetItemToResultListForView(newItem, this->_internalItems[relatedInternalItemIdx].getRelatedActionStatus(), indexToReplace);
}

void CheckResultListWindow::replaceItemAndRefreshStatusInFinalResultsAndInView(WebsiteListDatabaseItemModel newItem, int indexToReplace, ActionStatus newActionStatus)
{
    int relatedInternalItemIdx = this->__getInternalItemIndexByPresentedItemIndex(indexToReplace);
    if(relatedInternalItemIdx < 0) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid internal item index"));
        return;
    }
    this->_internalItems[relatedInternalItemIdx]._setFinalResultItem(newItem);
    this->_internalItems[relatedInternalItemIdx].setRelatedActionStatus(newActionStatus);
//    this->_finalResultItems.replace(indexToReplace, newItem);
//    this->_finalResultItemsActionStatuses[indexToReplace] = newActionStatus;

    QStandardItem *modelItem = new QStandardItem(this->getPresentableStatusTextByActionStatus(newActionStatus));
    _finalResultModelForView->setItem(indexToReplace, 3, modelItem);
}

QString CheckResultListWindow::getPresentableStatusTextByActionStatus(ActionStatus actionStatus) {
    QString statusText = tr("Invalid action status");

//    AS_NoAction = 0,
//    AS_Same,
//    AS_FirstCheck,
//    AS_KeepOriginalContent,
//    AS_UseNewContentButNotAnUpdate,
//    AS_UseNewContentAndMarkUpdate

    if(actionStatus == AS_NoAction) {
        statusText = tr("No action selected");
    }
    else if(actionStatus == AS_Same) {
        statusText = tr("Same - no action required");
    }
    else if(actionStatus == AS_FirstCheck) {
        statusText = tr("First checking - no action required");
    }
    else if(actionStatus == AS_KeepOriginalContent) {
        statusText = tr("Keep original");
    }
    else if(actionStatus == AS_UseNewContentButNotAnUpdate) {
        statusText = tr("Use new - but it's not an update");
    }
    else if(actionStatus == AS_UseNewContentAndMarkUpdate) {
        statusText = tr("Use new - it's an update");
    }

    return statusText;
}

//void CheckResultListWindow::saveResultButtonPressed()
//{
//    bool isRequiresFurtherActions = false;

//    for(int i = 0; i < _internalItems.size() && !isRequiresFurtherActions; i++) {
//        if(_internalItems[i].getRelatedActionStatus() == AS_NoAction) {
//            isRequiresFurtherActions = true;
//        }
//    }

//    // check whether further actions required for items
////    if(this->_finalResultItemsActionStatuses.contains(AS_NoAction)) {
//    if(isRequiresFurtherActions) {
//        QMessageBox::information(this, tr("Cannot do it"), tr("There's some items with no-action defined. Every item has to have an action."));
//        return;
//    }

//    QList<WebsiteListDatabaseItemModel> finalResultItemsToSave;
//    for(int i = 0; i < _internalItems.size(); i++) {
//        finalResultItemsToSave.append(_internalItems[i].getFinalResultItem());
//    }

//    if(this->_delegate != NULL) {
//        this->_delegate->saveFinalResultItems(finalResultItemsToSave);
//    }
//    else {
//        QMessageBox::information(this, tr("No delegate"), tr("Fatal: no delegate specified to save the results. Oh my..."));
//    }
//}

QList<QString> CheckResultListWindow::getUpdatedLinks()
{
//    int itemCnt = this->_finalResultItems.size();
//    if(itemCnt != this->_finalResultItemsActionStatuses.size()) {
//        QMessageBox::warning(this, tr("Error"), tr("The item count is not the same as the related item-actions count. Should be..."));
//        return;
//    }

    // temporarily this won't open the urls but it will save them to file
    // because somehow I couldn't manage to open multiple urls at the same time - at least 1 of them doesn't open correctly
    QList<QString> updatedAppLinks;

    for(int i = 0; i < _internalItems.size(); i++)
    {
        if(_internalItems[i].getRelatedActionStatus() == AS_UseNewContentAndMarkUpdate) {
            QString relatedUrlString;
            if(_internalItems[i].getFinalResultItem().getDownloadUrl().isEmpty()) {
                // no download url / link specified - open the reference url instead
                relatedUrlString = _internalItems[i].getFinalResultItem().getUrl();
            }
            else {
                // download url specified - open it
                relatedUrlString = _internalItems[i].getFinalResultItem().getDownloadUrl();
            }


            updatedAppLinks << relatedUrlString;

//            if(!QDesktopServices::openUrl(QUrl(relatedUrlString, QUrl::TolerantMode))) {
////            if(!QProcess::startDetached("explorer", QStringList() << relatedUrlString)) {
////            if(!QProcess::startDetached("\"" + relatedUrlString + "\"")) {
//                QMessageBox::warning(this, tr("Error"), tr("Cannot open the given link: %1").arg(relatedUrlString));
//            }
        }
    }

    return updatedAppLinks;
//    if(this->_delegate != NULL) {
//        this->_delegate->saveUpdatedAppLinks(updatedAppLinks);
//    }
//    else {
//        QMessageBox::information(this, tr("No delegate"), tr("Fatal: no delegate specified to save the results. Oh my..."));
//    }
}

void CheckResultListWindow::closeEvent(QCloseEvent *e)
{
    bool isIgnoreClose = false;

    if(this->_delegate == NULL) {
        QMessageBox::information(this, tr("No delegate"), tr("Fatal: no delegate specified to save the results. Oh my..."));
    }
    else {
        int saveDialogRes = QMessageBox::information(this, tr("Save result"), tr("Do you want to save the results?"), tr("Yes"), tr("No"));
        if(saveDialogRes == 0)
        {
            //
            // check whether every required action is set
            bool isRequiresFurtherActions = false;

            for(int i = 0; i < _internalItems.size() && !isRequiresFurtherActions; i++) {
                if(_internalItems[i].getRelatedActionStatus() == AS_NoAction) {
                    DLog("No action defined for app: ") << _internalItems[i].getOriginalItem().getAppName();
                    isRequiresFurtherActions = true;
                }
            }

            // check whether further actions required for items
            //    if(this->_finalResultItemsActionStatuses.contains(AS_NoAction)) {
            if(isRequiresFurtherActions) {
                QMessageBox::information(this, tr("Cannot do it"), tr("There's some item with no-action defined. Every item has to have an action (except the ones with 'No action required')."));
                isIgnoreClose = true;
            }
            else
            {
                // get the final-results
                QList<WebsiteListDatabaseItemModel> finalResultItemsToSave;
                for(int i = 0; i < _internalItems.size(); i++) {
                    finalResultItemsToSave.append(_internalItems[i].getFinalResultItem());
                }
                // and save them
                this->_delegate->saveFinalResultItems(finalResultItemsToSave);
            }
        }

        //
        // check the update-links
        QList<QString> updateLinkUrls = this->getUpdatedLinks();
        if(!updateLinkUrls.isEmpty())
        {
            int saveUpdateLinksDialogRes = QMessageBox::information(this, tr("Save update links"), tr("Do you want to save the updated-links (will be saved to 'updatedLinks.txt' in the directory of the input/output database)?"), tr("Yes"), tr("No"));
            if(saveUpdateLinksDialogRes == 0) {
                this->_delegate->saveUpdatedAppLinks(updateLinkUrls);
            }
        }
    }

    if(isIgnoreClose) {
        e->ignore();
    }
    else {
        e->accept();
    }

//    QMainWindow::closeEvent(e);
}
