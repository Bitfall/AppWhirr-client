#ifndef CLEVERAPPLICATIONICONPRESENTERSORTINGHANDLER_H
#define CLEVERAPPLICATIONICONPRESENTERSORTINGHANDLER_H

#include <QObject>

#include "widgetlistsnapshot.h"

class IdBasedWidgetBase;

//
// NOT THREAD SAFE
class CleverApplicationIconPresenterSortingDelegate : public QObject
{
    Q_OBJECT
public:
    explicit CleverApplicationIconPresenterSortingDelegate(QObject *parent = 0);

    //
    // before usage you have to call the _loadSortingDatabase method
    void _loadSortingDatabase(QString sortingDatabasePath);
    void _unloadSortingDatabase();

    //
    // Filters and Sorts the input by newFilter.
    // allItems: contains every item
    // currentItems: contains the current un-filtered ("presented") items -> mostly this is the
    //      processed result of a previous call to this function.
    void sortTheseByFilters(QList<IdBasedWidgetBase *> allItems, QString filter, bool isOnlyAutostartOnes, bool isOnlyHiddenOnes, bool isAllowHiddenOnes, bool isAllowNotInstalledOnes);

    void addOrSetValueForKey(QString filterKey, QString filterValue);

Q_SIGNALS:
    void sortingHappened(WidgetListSnapshot sortedItems);

private:
    bool isMachesFilter(QString source, QString filter);
    //
    // [!] call this only from the MAIN-THREAD
    void _sortTheseByFiltersImplementation(QList<IdBasedWidgetBase *> allItems, QString filter, bool isOnlyAutostartOnes, bool isOnlyHiddenOnes, bool isAllowHiddenOnes, bool isAllowNotInstalledOnes, bool isCaseSensitive, bool isCompareWithoutSpaces);

private:
    bool _isFilterMapLoadedFromDatabase;
    QMap<QString, QString> _filterKeyValueMap;

    QString _currSortingDatabasePath;
};

#endif // CLEVERAPPLICATIONICONPRESENTERSORTINGHANDLER_H
