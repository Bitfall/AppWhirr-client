#include "Common/Sorting/cleverapplicationiconpresentersortingdelegate.h"

#include "Application/appwhirrapplication.h"
#include "Common/Sorting/sortingdatabasemanager.h"
#include "Common/Widgets/idbasedwidgetbase.h"

#include "Browser/Content/Widget/appiconandbuttonpresenterwidget.h"

// global objects
//#include "Application/globalobjectregistry.h"
//#include "Application/LocalAppDatabaseServer/localappdatabaseserver.h"

// debug and utility
#include <QDebug>
#include <QMutex>

CleverApplicationIconPresenterSortingDelegate::CleverApplicationIconPresenterSortingDelegate(QObject *parent) :
    QObject(parent),
    _isFilterMapLoadedFromDatabase(false)
{
}

bool CleverApplicationIconPresenterSortingDelegate::isMachesFilter(QString source, QString filter)
{
    if( source.contains(filter) )
    {
        return true;
    }

    return false;
}

void CleverApplicationIconPresenterSortingDelegate::sortTheseByFilters(QList<IdBasedWidgetBase *> allItems, QString filter, bool isOnlyAutostartOnes, bool isOnlyHiddenOnes, bool isAllowHiddenOnes, bool isAllowNotInstalledOnes) {
    this->_sortTheseByFiltersImplementation(allItems, filter, isOnlyAutostartOnes, isOnlyHiddenOnes, isAllowHiddenOnes, isAllowNotInstalledOnes, false, true);
}

void CleverApplicationIconPresenterSortingDelegate::_sortTheseByFiltersImplementation(QList<IdBasedWidgetBase *> allItems, QString filterText, bool isOnlyAutostartOnes, bool isOnlyHiddenOnes, bool isAllowHiddenOnes, bool isAllowNotInstalledOnes, bool isCaseSensitive, bool isCompareWithoutSpaces)
{    
    WidgetListSnapshot resultItems;
    QList<QWidget *> resultIconPresenterList;

    QString normalizedFilterText = filterText;
    if(!isCaseSensitive) {
        normalizedFilterText = normalizedFilterText.toLower();
    }

    if(isCompareWithoutSpaces) {
        normalizedFilterText = normalizedFilterText.replace(" ", "");
    }


    //
    // first, filter the items
    {
        QList<IdBasedWidgetBase *>::const_iterator it = allItems.begin();
        QList<IdBasedWidgetBase *>::const_iterator eit = allItems.end();
        for( ; it != eit; ++it )
        {
            bool isAddCurrItemToResultList = false;
            AppIconAndButtonPresenterWidget *castedIconPresenterWidget = dynamic_cast<AppIconAndButtonPresenterWidget *>( (*it) );
            if(castedIconPresenterWidget == NULL) {
                WLog("Cannot cast the widget to icon-presenter-widget. Cannot sort it.");
            }
            else {
                QString currItem = castedIconPresenterWidget->getRelatedAppName();
                if(isCompareWithoutSpaces) {
                    currItem = currItem.replace(" ", "");
                }
                if(!isCaseSensitive) {
                    currItem = currItem.toLower();
                }

                if(isMachesFilter(currItem, normalizedFilterText))
                {
                    // maches the filter-text, filter by other filters

                    bool isHidden = castedIconPresenterWidget->getRelatedAppDescriptionModel().getDynamicDescription_ReadOnly()->getIsHidden();

                    if(!isHidden) {
                        isAddCurrItemToResultList = true;
                    } else if(isAllowHiddenOnes) {
                        isAddCurrItemToResultList = true;
                    }

                    if(isAddCurrItemToResultList) {
                        if(isOnlyHiddenOnes && !isHidden) {
                            isAddCurrItemToResultList = false;
                        }
                    }


                    if(isAddCurrItemToResultList) {
                        if(isOnlyAutostartOnes) {
                            bool isStartThisAppWhenClientStarts = castedIconPresenterWidget->getRelatedAppDescriptionModel().getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts();
                            if(!isStartThisAppWhenClientStarts) {
                                isAddCurrItemToResultList = false;
                            }
                        }
                    }

                    if(isAddCurrItemToResultList) {
                        if(!isAllowNotInstalledOnes) {
                            AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum currState = castedIconPresenterWidget->getRelatedAppDescriptionModel().getDynamicDescription_ReadOnly()->getAppState();
                            if((int)currState < (int)AppWhirr::ApplicationGrabStates::Installed) {
                                isAddCurrItemToResultList = false;
                            }
                        }
                    }
                }
            }

            if(isAddCurrItemToResultList) {
                resultIconPresenterList << *it;
            }
        }
    }



    //
    // sort them
    {
        //    QString filterMatchItem;
        //    SortingDatabaseManager *sortingDbManager = new SortingDatabaseManager;
        //    bool isSuccess = sortingDbManager->getValueForKey(filterText, filterMatchItem);
        //    delete sortingDbManager;

        QString filterMatchItem = this->_filterKeyValueMap.value(filterText, QString());

        // filter match test
        if(!filterMatchItem.isEmpty())
        {
            // filter match found!!
            qDebug("!! filter match found !!");
            int idx = -1;
            IdBasedWidgetBase *foundFilterMatchItem = NULL;

            // search
            {
                int foundItemCnt = resultIconPresenterList.size();
                for(int i = 0; i < foundItemCnt && idx == -1; i++) {
                    AppIconAndButtonPresenterWidget *castedItem = dynamic_cast<AppIconAndButtonPresenterWidget *>(resultIconPresenterList[i]);
                    if(castedItem != NULL &&
                       castedItem->getRelatedAppName() == filterMatchItem) {
                        idx = i;
                        foundFilterMatchItem = castedItem;
                    }
                }
            }

            if(idx != -1)
            {
                // move to the front
                resultIconPresenterList.removeAt(idx);
                resultIconPresenterList.insert(0, foundFilterMatchItem);
            }
        }
    }


    // and Q_EMIT and update
    resultItems._setItems(resultIconPresenterList);
    Q_EMIT sortingHappened(resultItems);

    //return resultItems;
}

void CleverApplicationIconPresenterSortingDelegate::addOrSetValueForKey(QString filterKey, QString filterValue)
{
    if(_currSortingDatabasePath.isEmpty()) {
        WLog("Sorting database path is empty - probably you forgot to call _loadSortingDatabase");
        return;
    }

    this->_filterKeyValueMap[filterKey] = filterValue;
    SortingDatabaseManager *sortingDbManager = new SortingDatabaseManager(_currSortingDatabasePath);
    sortingDbManager->setValueForKey(filterKey, filterValue);
    delete sortingDbManager;
}

void CleverApplicationIconPresenterSortingDelegate::_loadSortingDatabase(QString sortingDatabasePath)
{
    if(sortingDatabasePath.isEmpty()) {
        WLog("Given sorting database path is empty. Cannot load it.");
        return;
    }

    if(!this->_isFilterMapLoadedFromDatabase)
    {
        QMutex mutex;
        mutex.lock();

        if(!this->_isFilterMapLoadedFromDatabase) {
            // load
            SortingDatabaseManager *sortingDbManager = new SortingDatabaseManager(sortingDatabasePath);
            bool isSuccess = sortingDbManager->getAllFilterKeyValuePairs(this->_filterKeyValueMap);
            delete sortingDbManager;

            // flag
            if(isSuccess) {
                this->_currSortingDatabasePath = sortingDatabasePath;
                this->_isFilterMapLoadedFromDatabase = true;
            }
            else {
                WLog("Couldn't get the filter key-values. Probably invalid database-path.");
            }
        }

        mutex.unlock();
    }
}

void CleverApplicationIconPresenterSortingDelegate::_unloadSortingDatabase()
{
    if(this->_isFilterMapLoadedFromDatabase)
    {
        QMutex mutex;
        mutex.lock();

        if(this->_isFilterMapLoadedFromDatabase) {
            this->_isFilterMapLoadedFromDatabase = false;
            this->_filterKeyValueMap.clear();
        }

        mutex.unlock();
    }
}
