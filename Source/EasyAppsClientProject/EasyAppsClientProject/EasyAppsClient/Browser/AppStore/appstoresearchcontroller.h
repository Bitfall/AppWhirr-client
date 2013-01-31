#ifndef APPSTORESEARCHCONTROLLER_H
#define APPSTORESEARCHCONTROLLER_H

#include <QObject>
#include <QMap>

class ApplicationStoreAppWidget;

class AppStoreSearchController : public QObject
{
    Q_OBJECT
public:
    explicit AppStoreSearchController(QObject *parent = 0);

public Q_SLOTS:
    void storeThisSearchFilterTextAsCurrent(QString searchFilterText);
    void searchByTheStoredFilterText();

    // ----------------
    // --- cacheing ---

    //
    // returns the related app-store-item-presenter widget for the given appId
    //  or returns NULL if not found
    ApplicationStoreAppWidget *getPresenterItemFromCacheByAppId(QString appId) const;
    //
    void storePresenterItemInCache(QString appId, ApplicationStoreAppWidget *relatedAppStoreSearchPresenter);
    //
    // will only clear the mapping (the cache) - WON'T delete the cached presenter-widgets!
    void clearCachedItems();
    //
    int getCachedItemCount() const;
    //
    // will remove every item not in the currently-used list
    //  if isAlsoDeleteRelatedPresenterItems is true then it will also delete the related presenter-items after remove
    void removeUnusedItems(bool isAlsoDeleteRelatedPresenterItems);

    //
    // the currently used list manager
    //

    void addThisPresenterItemToCurrentlyUsedOnes(QString appId, ApplicationStoreAppWidget *appStoreSearchPresenterItem);
    void clearCurrentlyUsedPresenterItemList();
    int _getCurrentlyUsedItemCnt();

Q_SIGNALS:
    void __performSearchByFilterText(QString searchFilterText);
    void __searchFilterChangedButNotYetPerformed(QString currentFilterText);

private:
    void _startSearchByCurrentSearchFilterText();

private:
    QString _currentSearchFilterText;
    QString _lastPerformedSearchFilterText; // stores the last performed one -> if seachByTheStoredFilterText() is called then the current search filter text will be "performed"

    QMap<QString, ApplicationStoreAppWidget *> _appIdToCachedItemPresenterMap;
    QMap<QString, bool> _currentlyUsedPresenterItems;
};

#endif // APPSTORESEARCHCONTROLLER_H
