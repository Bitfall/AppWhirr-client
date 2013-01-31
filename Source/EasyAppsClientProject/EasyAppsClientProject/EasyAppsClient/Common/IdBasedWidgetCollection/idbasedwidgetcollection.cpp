#include "idbasedwidgetcollection.h"

IdBasedWidgetCollection::IdBasedWidgetCollection(QObject *parent) :
    QObject(parent)
{
}

void IdBasedWidgetCollection::_removeAll(bool isDeleteAsWell)
{
    if(isDeleteAsWell) {
        int itemCnt = this->_allItems.size();
        for(int i = 0; i < itemCnt; i++) {
            delete this->_allItems[i];
            this->_allItems[i] = NULL;
        }
    }

    this->_allItems.clear();
}

void IdBasedWidgetCollection::add(IdBasedWidgetBase *item)
{
//    this->allIconTitles << iconPresenter->getIconModel().getIconTitle();
//    iconTitleToPresenterWidgetMap[iconPresenter->getIconModel().getIconTitle()] = iconPresenter;
    this->_allItems << item;
}

void IdBasedWidgetCollection::_refreshListOrderByIdList(QList<QString> newOrderedIdList)
{
//    this->allIconTitles.clear();

    QList<IdBasedWidgetBase *> newOrderedItemsList;

    int newOrderedItemsCnt = newOrderedIdList.size();
    for(int i = 0; i < newOrderedItemsCnt; i++) {
#if 0
        if(iconTitleToPresenterWidgetMap.contains(newOrderedIconModelList[i].getIconTitle())) {
            this->allIconTitles << newOrderedIconModelList[i].getIconTitle();
        } else {
            qWarning(" ! The given item was not found in the iconTitle-to-presenter map, cannot be added to the all-items list!");
        }
#endif
        IdBasedWidgetBase *foundItem = NULL;
        if( (foundItem = this->getFirstById(newOrderedIdList[i])) != NULL) {
            // found - OK
            newOrderedItemsList.append( foundItem );
        } else {
            WLog(" ! The given item was not found in the inner list, cannot be added! Probably you forgot to add it before you called this reordering method. It won't be added to the list.");
        }
    }

    this->_allItems = newOrderedItemsList;
}

void IdBasedWidgetCollection::_setItemsFromWidgetList(QList<QWidget *> newItems)
{
    this->_allItems.clear();
    for(int i = 0; i < newItems.size(); i++) {
        IdBasedWidgetBase *castedItem = dynamic_cast<IdBasedWidgetBase *>(newItems[i]);
        if(castedItem == NULL) {
            WLog("Cannot cast!");
        }
        else {
            this->_allItems.append(castedItem);
        }
    }
}

QList<IdBasedWidgetBase *> IdBasedWidgetCollection::getAll() {
    return this->_allItems;
}

QList<QWidget *> IdBasedWidgetCollection::_getAllAsWidgetList() {
    // convert the id-based list to simple, widget list
    QList<QWidget *> asWidgetList;
    int widgetCnt = this->_allItems.size();
    for(int i = 0; i < widgetCnt; i++) {
        asWidgetList << this->_allItems[i];
    }

    return asWidgetList;
}

QList<QString> IdBasedWidgetCollection::_getAllIds()
{
    // convert the id-based list to simple, widget list
    QList<QString> asIdList;
    int widgetCnt = this->_allItems.size();
    for(int i = 0; i < widgetCnt; i++) {
        asIdList << this->_allItems[i]->getId();
    }

    return asIdList;
}

IdBasedWidgetBase *IdBasedWidgetCollection::getFirstById(QString id)
{
    IdBasedWidgetBase *ret = NULL;
    int allItemCnt = this->_allItems.size();
    for(int i = 0; (i < allItemCnt) && (ret == NULL); i++) {
        if(this->_allItems[i]->getId() == id) {
            ret = this->_allItems[i];
        }
    }

    return ret;
}

QList<IdBasedWidgetBase *> IdBasedWidgetCollection::getAllById(QString id)
{
    QList<IdBasedWidgetBase *> ret;
    int allItemCnt = this->_allItems.size();
    for(int i = 0; i < allItemCnt; i++) {
        if(this->_allItems[i]->getId() == id) {
            ret.append(this->_allItems[i]);
        }
    }

    return ret;
}

#if 0
IconPresenterWidget *IdBasedWidgetCollection::getIconPresenterWidgetByTitle(QString title)
{
    if(!iconTitleToPresenterWidgetMap.contains(title)) {
        return NULL;
    }

#if 0
    QMap<QString, IconPresenterWidget *>::iterator it = iconTitleToPresenterWidgetMap.find(title);
    if(it == iconTitleToPresenterWidgetMap.end()) {
        return NULL;
    }

    return it.value();
#endif

    return iconTitleToPresenterWidgetMap[title];
}
#endif
