#include "appstoresearchcontroller.h"

#include "Browser/Content/Widget/applicationstoreappwidget.h"

#define IS_START_SEARCH_IMMEDIATELLY_WHEN_SEARCH_FILTER_CHANGES true

AppStoreSearchController::AppStoreSearchController(QObject *parent) :
    QObject(parent)
{
}

void AppStoreSearchController::storeThisSearchFilterTextAsCurrent(QString searchFilterText)
{
    this->_currentSearchFilterText = searchFilterText;

    if(IS_START_SEARCH_IMMEDIATELLY_WHEN_SEARCH_FILTER_CHANGES) {
        this->_startSearchByCurrentSearchFilterText();
    }
    else {
        Q_EMIT __searchFilterChangedButNotYetPerformed(searchFilterText);
    }
}

void AppStoreSearchController::searchByTheStoredFilterText()
{
    // tmp workaround - makes a bit more convinient to start search if the user pops-back to Store page
    this->_startSearchByCurrentSearchFilterText();
//    if(!IS_START_SEARCH_IMMEDIATELLY_WHEN_SEARCH_FILTER_CHANGES)
//    {
//        this->_startSearchByCurrentSearchFilterText();
//    }
//    else {
//        // don't search here - search is started when search-filter-text changed
//    }
}

void AppStoreSearchController::_startSearchByCurrentSearchFilterText()
{
    Q_EMIT __performSearchByFilterText(_currentSearchFilterText);
    _lastPerformedSearchFilterText = _currentSearchFilterText;
}

// ----------------
// --- cacheing ---

ApplicationStoreAppWidget *AppStoreSearchController::getPresenterItemFromCacheByAppId(QString appId) const
{
    if(this->_appIdToCachedItemPresenterMap.contains(appId)) {
        return this->_appIdToCachedItemPresenterMap.value(appId, NULL);
    }

    return NULL;
}

void AppStoreSearchController::storePresenterItemInCache(QString appId, ApplicationStoreAppWidget *relatedAppStoreSearchPresenter)
{
    this->_appIdToCachedItemPresenterMap[appId] = relatedAppStoreSearchPresenter;
}

void AppStoreSearchController::clearCachedItems()
{
    this->_appIdToCachedItemPresenterMap.clear();
}

int AppStoreSearchController::getCachedItemCount() const
{
    return this->_appIdToCachedItemPresenterMap.size();
}

void AppStoreSearchController::removeUnusedItems(bool isAlsoDeleteRelatedPresenterItems)
{
    DLog("removeUnusedItems");
    QList<QString> itemIdsToRemoveFromCache;

    QMap<QString, ApplicationStoreAppWidget *>::const_iterator i = _appIdToCachedItemPresenterMap.constBegin();
    while (i != _appIdToCachedItemPresenterMap.constEnd())
    {
        QString currAppId = i.key();

        if(!_currentlyUsedPresenterItems.contains(currAppId)) {
            // mark for remove
            itemIdsToRemoveFromCache.append(currAppId);
        }

        ++i;
    }

    //
    // remove them
    for(int i = 0; i < itemIdsToRemoveFromCache.size(); i++) {
        QString currAppId = itemIdsToRemoveFromCache[i];
        ApplicationStoreAppWidget *relatedCachedPresenterItem = _appIdToCachedItemPresenterMap.value(currAppId, NULL);

        _appIdToCachedItemPresenterMap.remove(currAppId);
        if(relatedCachedPresenterItem == NULL) {
            WLog("Item found in cache but the related cached presenter item is NULL: ") << currAppId;
        }
        else {
            // found
            DLog("Related presenter item found for appId: ") << currAppId;

            if(isAlsoDeleteRelatedPresenterItems) {
                DLog("Related presenter item will be deleted as well.");
                delete relatedCachedPresenterItem;
            }
        }
    }
}

void AppStoreSearchController::addThisPresenterItemToCurrentlyUsedOnes(QString appId, ApplicationStoreAppWidget *appStoreSearchPresenterItem)
{
    _currentlyUsedPresenterItems[appId] = true;
}

void AppStoreSearchController::clearCurrentlyUsedPresenterItemList()
{
    _currentlyUsedPresenterItems.clear();
}

int AppStoreSearchController::_getCurrentlyUsedItemCnt()
{
    return this->_currentlyUsedPresenterItems.size();
}
