#include "menumanager.h"

#include "menupresenterwidgetbase.h"
#include "menuitemholderwidgetbase.h"
#include "menuitemwidget.h"

// config
#include "Settings/configmanager.h"

MenuManager::MenuManager(MenuPresenterWidgetBase *menuPresenterWidget, QObject *parent) :
    QObject(parent),
  _menuPresenterWidget(menuPresenterWidget)
{
}

void MenuManager::addRootItem(MenuItemWidget *item)
{
    connect(item, SIGNAL(itemClick(QString)), this, SLOT(activateItemById(QString)));
    MenuItemHolderWidgetBase *newElem = this->_menuPresenterWidget->createMenuItemHolderWidgetByMenuItemWidget(item);
    rootMenuItems << newElem;

    if(this->_menuPresenterWidget) {
        this->_menuPresenterWidget->addRootMenuItemHolderWidget(newElem);
    }
    else {
        WLog("No menu presenter widget added to MenuManager - required!");
    }
}

MenuItemWidget *MenuManager::_getCurrentTopActiveItem()
{
    if(currentActiveItemElementPath.isEmpty())
    {
        return NULL;
    }

    return currentActiveItemElementPath.last()->getRelatedMenuItem();
}

void MenuManager::addCustomWidgetToItem(QWidget *customWidget, QString itemId)
{
    MenuItemWidget *relatedMenuItemWidget = this->getItemById(itemId);
    if(relatedMenuItemWidget == NULL) {
        WLogS << "The requested itemId is not found, cannot add the custom widget. ID: " << itemId;
        return;
    }

    this->_addCustomWidgetToItem(customWidget, relatedMenuItemWidget);
}

void MenuManager::_addCustomWidgetToItem(QWidget *customWidget, MenuItemWidget *item)
{
    MenuItemHolderWidgetBase *relatedItemElement = getItemElementByItem(item);
    if(relatedItemElement == NULL)
    {
        WLog("related item element not found");
        return;
    }

    relatedItemElement->addCustomWidget(customWidget);
}

bool MenuManager::addChildToItem(MenuItemWidget *newChildItem, QString theParentItemId, QWidget *customWidgetToThisItem, bool isAddItWithAnimation)
{
    MenuItemWidget *parentMenuItemWidget = this->getItemById(theParentItemId);
    if(parentMenuItemWidget == NULL) {
        WLogS << "The requested parentMenuItem is not found, cannot add the child item. ID: " << theParentItemId;
        return false;
    }

    return this->__addChildToItem(newChildItem, parentMenuItemWidget, customWidgetToThisItem, isAddItWithAnimation);
}

bool MenuManager::__addChildToItem(MenuItemWidget *newChildItem, MenuItemWidget *theParentItem, QWidget *customWidgetToThisItem, bool isAddItWithAnimation)
{
    MenuItemHolderWidgetBase *parentItemElem = getItemElementByItem(theParentItem);
    if(parentItemElem == NULL)
    {
        WLog("parent cannot be found - cannot add the child");
        return false;
    }

    connect(newChildItem, SIGNAL(itemClick(QString)), this, SLOT(activateItemById(QString)));
    MenuItemHolderWidgetBase *newElem = this->_menuPresenterWidget->createMenuItemHolderWidgetByMenuItemWidget(newChildItem);

    if(customWidgetToThisItem != NULL)
    {
        newElem->addCustomWidget(customWidgetToThisItem);
    }


    parentItemElem->addChildToLayout(newElem);

    if(isAddItWithAnimation)
    {
        newElem->hideImmediatelly();
        newElem->_showAnimated();
    }

    connect(newElem, SIGNAL(willDeleteItselfSignal(MenuItemHolderWidgetBase*)), this, SLOT(removeThisItemBecauseItWillDeleteItself(MenuItemHolderWidgetBase*)));
//    connect(newElem, SIGNAL(_itemForceUpdated()), this, SLOT(__forceUpdateMenuPresenterWidget()));

    return true;
}

void MenuManager::removeThisItemBecauseItWillDeleteItself(MenuItemHolderWidgetBase *itemElem)
{
    FLAG_FOR_REVIEW_WITH_HINT("This method can be dangerous - it may access deleted items!");

#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("removeThisItemBecauseItWillDeleteItself");
#endif

    if(this->getIndexInCurrentActiveItemPath(itemElem->getRelatedMenuItem()) >= 0) {
        // item is in the current active item path
        currentActiveItemElementPath.removeAll(itemElem);
    }
    else {
        // not in the current item path
        bool isFound = false;
        int rootCnt = this->rootMenuItems.size();
        for(int i = 0; i < rootCnt && !isFound; i++) {
            QList<MenuItemHolderWidgetBase *> pathToItem;
            if( getPathToThisItem(rootMenuItems[i], itemElem->getRelatedMenuItem(), pathToItem) )
            {
                isFound = true;
                _removeItem_Recursive(rootMenuItems[i], itemElem->getRelatedMenuItem()->getId());
            }
        }

        if (!isFound){
            // not found in any stacks
            WLog("Cannot find the item in any stacks: ") << itemElem->getRelatedMenuItem()->getId();
        }
    }


#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("finished removing");
#endif
}

bool MenuManager::_removeItem_Recursive(MenuItemHolderWidgetBase *head, QString id) {

    if(head->getRelatedMenuItem()->getId() == id)
    {
        FLAG_FOR_REVIEW_WITH_HINT("A root item will be never removed. If it'll be required this has to be refactored.");
        return true;
    } else {
        bool isFound = false;
        QList<MenuItemHolderWidgetBase *> tmpChildren = head->getChildrenElements();
        int cnt = tmpChildren.size();
        for(int i = 0; i < cnt; i++)
        {
            isFound = _removeItem_Recursive(tmpChildren[i], id);
            if(isFound)
            {
                AW_QOBJECT_SAFE_DELETE(tmpChildren[i]);
                // false means the parent shouldn't delete this head (because delete is performed here)
                return false;
            }
        }
    }

    return false;
}

void MenuManager::highlightItemById(QString id)
{
    this->_activateItemById(id, true);
}

// slot
void MenuManager::activateItemById(QString sourceMenuItemId)
{
    this->_activateItemById(sourceMenuItemId, false);
}

void MenuManager::_activateItemById(QString sourceMenuItemId, bool isOnlyHighlightIt)
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("activateThisItem");
#endif

    MenuItemWidget *sourceMenuItem = this->getItemById(sourceMenuItemId);
    if(sourceMenuItem == NULL)
    {
        WLog("Item id is empty. Cannot be activated.");
        return;
    }

    MenuItemHolderWidgetBase *containerElem = getItemElementByItem(sourceMenuItem);
    if(containerElem != NULL)
    {
        if(!currentActiveItemElementPath.isEmpty())
        {
            if( currentActiveItemElementPath.last()->getRelatedMenuItem() == sourceMenuItem )
            {
                // the clicked element is the currently selected one
                // so (at least now) don't do anything
                DLog("This is the current active menu item.");
                if(ConfigManager::getMenuItemIsActivateIfThatsTheCurrentOneWhenRequested()) {
                    sourceMenuItem->_emitItemBecomeTheTopActiveOneSignal();
                }
                return;
            }
        }
        //qDebug("found");
        //qDebug(containerElem->getRelatedMenuItem()->getId().toAscii());
        int idxInPath = getIndexInCurrentActiveItemPath(sourceMenuItem);
        if(idxInPath != -1)
        {
            // the clicked item is in the currently active item-path

            //  change only the path
            this->activateCurrentItemPathToThisIndex(idxInPath);
        }
        else
        {
            // this item is not in the active-item-path

            //  this will be a new item -> fill it's sub-items
            //Q_EMIT fillSubItemsToThisItem(source);
            if(!isOnlyHighlightIt) {
//                source->_emitInitializeDataToThisItemBecauseItWillBecomeTopActive();
                //source->_emitFillSubItemsToThisItemSignal();
            }

            // the clicked item is not in the currently active item-path
            // so it has to be a root item or a child of the currently active elem

            bool isFound = false;

            // check: is it a child of an item in the active-item-path?
            int activeItemPathCnt = currentActiveItemElementPath.size();
            for(int i = 0; i < activeItemPathCnt && !isFound; i++)
            {
                QList<MenuItemHolderWidgetBase *> currentlyActiveItemElementChildren = currentActiveItemElementPath[i]->getChildrenElements();
                int childrenCnt = currentlyActiveItemElementChildren.size();
                for(int j = 0 ; j < childrenCnt && !isFound; j++)
                {
                    if(currentlyActiveItemElementChildren[j]->getRelatedMenuItem() == sourceMenuItem)
                    {
                        isFound = true;
                        //addThisItemElementToActiveItemElementPathAndActivate(currentlyActiveItemElementChildren[i]);
                        stepBackToThisIndexInActiveItemPathAndAddAndActivateThisItemElement(i, currentlyActiveItemElementChildren[j]);
                    }
                }
            }


            // check: is it a root item?
            if(!isFound)
            {
                int rootCnt = rootMenuItems.size();
                for(int i = 0; i < rootCnt && !isFound; i++)
                {
                    if(rootMenuItems[i]->getRelatedMenuItem() == sourceMenuItem)
                    {
                        isFound = true;
                        activateThisRootItem(rootMenuItems[i]);
                    }

                    // search in children as well
                    QList<MenuItemHolderWidgetBase *> pathToItem;
                    if( getPathToThisItem(rootMenuItems[i], sourceMenuItem, pathToItem) )
                    {
                        isFound = true;
                        activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, pathToItem);
                    }
                }
            }

            // if still not found
            if(!isFound)
            {
                WLog("Clicked item was not found in the root items as well. What the heck? Something is incorrect.");
            }
            else
            {
                if(!isOnlyHighlightIt) {
//                    source->_emitFillSubItemsToThisItemSignal();
                }
            }
        }

        if(!isOnlyHighlightIt) {
            //Q_EMIT itemClickedSignal(source);
            sourceMenuItem->_emitItemBecomeTheTopActiveOneSignal();
        }
        return;
    }

    WLog("the clicked item not found!!!");
}

bool MenuManager::getPathToThisItem(MenuItemHolderWidgetBase *head, MenuItemWidget *itemToSearchFor, QList<MenuItemHolderWidgetBase *> &refPath)
{
    // backtrack algorithm

//    DLog("Head ID: ") << head->getRelatedMenuItem()->getId();
//    DLog("Search ID: ") << itemToSearchFor->getId();

    refPath << head;

    if(head->getRelatedMenuItem() == itemToSearchFor)
    {
        return true;
    }

    QList<MenuItemHolderWidgetBase *> children = head->getChildrenElements();
    int childCnt = children.size();
//    DLog("Child cnt: ") << childCnt;
    bool isFound = false;
    for(int i = 0; i < childCnt && !isFound; i++)
    {
        if(getPathToThisItem(children[i], itemToSearchFor, refPath))
        {
            isFound = true;
        }
    }

    if(isFound)
    {
        return true;
    } else {
        refPath.removeOne(head);
        return false;
    }
}

void MenuManager::stepBackToThisIndexInActiveItemPathAndAddAndActivateThisItemElement(int index, MenuItemHolderWidgetBase *itemElement)
{
    int itemCnt = currentActiveItemElementPath.size();

    if(index < 0 || index > itemCnt-1)
    {
        WLog("invalid index - stepBackToThisIndexInActiveItemPathAndAddAndActivateThisItemElement");
    }

#if 0
    int itemCnt = currentActiveItemElementPath.size();
    for(int i = itemCnt-1; i > index; i--)
    {
        currentActiveItemElementPath[i]->hideAnimated();
        currentActiveItemElementPath.removeAt(i);
    }

    addThisItemElementToActiveItemElementPathAndActivate(itemElement);
#endif

    QList<MenuItemHolderWidgetBase *> newItemPath;
    for(int i = 0; i <= index; i++)
    {
        newItemPath << currentActiveItemElementPath[i];
    }
    newItemPath << itemElement;

    activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, newItemPath);
}

void MenuManager::addThisItemElementToActiveItemElementPathAndActivate(MenuItemHolderWidgetBase *itemElement)
{
#if 0
    currentActiveItemElementPath << itemElement;
    activateCurrentItemPathToThisIndex(currentActiveItemElementPath.size() - 1);
#endif
    QList<MenuItemHolderWidgetBase *> newItemPath;
    for(int i = 0; i < currentActiveItemElementPath.size(); i++)
    {
        newItemPath << currentActiveItemElementPath[i];
    }
    newItemPath << itemElement;
    activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, newItemPath);
}

void MenuManager::activateCurrentItemPathToThisIndex(int index)
{
    int itemCnt = currentActiveItemElementPath.size();

    if(index < 0 || index > itemCnt-1)
    {
        WLog("invalid index - activateCurrentItemPathToThisIndex");
        return;
    }

    QList<MenuItemHolderWidgetBase *> newItemPath;
    for(int i = 0; i <= index; i++)
    {
        newItemPath << currentActiveItemElementPath[i];
    }
    activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, newItemPath);
}

void MenuManager::activateThisRootItem(MenuItemHolderWidgetBase *itemElement)
{
    QList<MenuItemHolderWidgetBase *> newItemPath;
    newItemPath << itemElement;
    activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, newItemPath);
}

void MenuManager::activateCurrentItemPathBySnapshotChange(QList<MenuItemHolderWidgetBase *> sourceSnapshot, QList<MenuItemHolderWidgetBase *> targetSnapshot)
{
    // TEST
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("# source");
    printThisListOfMenuItemElements(sourceSnapshot);

    DLog("# target");
    printThisListOfMenuItemElements(targetSnapshot);
#endif
    //

    // first, filter out the "in-destroying-process" elements from the target snapshot
    QList<MenuItemHolderWidgetBase *> tmpRemoveList;
    for(int i = 0; i < targetSnapshot.size(); i++)
    {
        if(targetSnapshot[i]->getIsDestroyingItself())
        {
            tmpRemoveList << targetSnapshot[i];
        }
    }

    for(int i = 0; i < tmpRemoveList.size(); i++)
    {
        targetSnapshot.removeAll(tmpRemoveList[i]);
    }

#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("# reduced target");
    printThisListOfMenuItemElements(targetSnapshot);
#endif

    int sourceSnapshotCnt = sourceSnapshot.size();
    int targetSnapshotCnt = targetSnapshot.size();

    // first compare source to target
    // - remove the ones found only in source but not in target
    for(int i = 0; i < sourceSnapshotCnt; i++)
    {
        bool isFound = false;
        for(int j = 0; j < targetSnapshotCnt && !isFound; j++)
        {
            if(sourceSnapshot[i] == targetSnapshot[j])
            {
                isFound = true;
            }
        }

        if(!isFound)
        {
            // not found in target, so remove it (or deactivate if it's a root item)
            if(i == 0) {
                // index 0 = root item, don't hide or delete it!
                sourceSnapshot[0]->deactivateAnimated();
            } else {
                sourceSnapshot[i]->hideAnimated(false);
//                sourceSnapshot[i]->hideAndDeleteAfterAnimated(false);
                // !!! maybe this can cause problems, have to test it. but this makes the id-based-mechanizm more secure
//                this->removeThisItemFromActivePathBecauseItWillDeleteItself(sourceSnapshot[i]);
            }
            //sourceSnapshot.removeAt(i);
        }
    }

    // second: compare target to source
    // - add the ones found only in target but not in source
    for(int i = 0; i < targetSnapshotCnt; i++)
    {
        bool isFound = false;
        for(int j = 0; j < sourceSnapshotCnt && !isFound; j++)
        {
            if(targetSnapshot[i] == sourceSnapshot[j])
            {
                isFound = true;
            }
        }

        if(!isFound)
        {
            // not found in source, so add it
            // don't do anything with this if it's destroying itself!!!
            targetSnapshot[i]->_showAnimated();
        }
    }

    // and apply the target as current
    currentActiveItemElementPath = targetSnapshot;

    // and activate them
    int currItemPathCnt = currentActiveItemElementPath.size();
    for(int i = currItemPathCnt - 1; i >= 0; i--)
    {
        //currentActiveItemElementPath[i]->showImmediatelly(); // bit of a heck
        currentActiveItemElementPath[i]->activateWithLevelAnimated(currItemPathCnt - 1 - i);
    }

#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("activateCurrentItemPathBySnapshotChange finished");
#endif
}

int MenuManager::getIndexInCurrentActiveItemPath(MenuItemWidget *item)
{
    int cnt = currentActiveItemElementPath.size();
    for(int i = 0; i < cnt; i++)
    {
        if(currentActiveItemElementPath[i]->getRelatedMenuItem() == item)
        {
            return i;
        }
    }

    // not found
    return -1;
}

//void MenuManager::deactivateItemInNavigationPathById(QString id)
//{
//    _removeOrDeactivateItemFromNavigationPathById(id, false);
//}

//void MenuManager::removeItemInNavigationPathById(QString id)
//{
//    if(id.isEmpty())
//    {
//        return;
//    }

//    _removeOrDeactivateItemFromNavigationPathById(id, true);
//}

void MenuManager::removeItemById(QString id)
{
    if(id.isEmpty())
    {
        return;
    }

#if 0
    MenuItemWidget *item = this->getItemById(id);
    if(item != NULL)
    {
        MenuItemHolderWidgetBase *element = this->getItemElementByItem(item);
        element->hideAndDeleteAfterAnimated(true);
        this->removeThisItemFromActivePathBecauseItWillDeleteItself(element);
    }
#endif

    _removeItemById(id);
}

void MenuManager::_removeItemById(QString id)
{
    // !!!
    // probably this should also check the child-items as well!

    QList<MenuItemHolderWidgetBase *> theNewPathItemRemoved;

    MenuItemHolderWidgetBase *theItemElementToTheId = NULL;

    bool isItemFoundInActiveItemPath = false;
//    bool isItemFoundAsChildItem = false;

    // check in current-active-item-path
    int currItemPathCnt = currentActiveItemElementPath.size();
    for(int i = 0; i < currItemPathCnt && !isItemFoundInActiveItemPath; i++)
    {
        if(currentActiveItemElementPath[i]->getRelatedMenuItem()->getId() == id)
        {
            theItemElementToTheId = currentActiveItemElementPath[i];
            isItemFoundInActiveItemPath = true;
        }
        else
        {
            // check also the children
            QList<MenuItemHolderWidgetBase *> children = currentActiveItemElementPath[i]->getChildrenElements();
            for(int j = 0; j < children.size() && !isItemFoundInActiveItemPath; j++)
            {
                if(children[j]->getRelatedMenuItem()->getId() == id)
                {
                    theItemElementToTheId = children[j];
                    isItemFoundInActiveItemPath = true;
                }
            }

            theNewPathItemRemoved << currentActiveItemElementPath[i];
        }
    }

    if(!isItemFoundInActiveItemPath/* && !isItemFoundAsChildItem*/)
    {
        // not found in the current active-item-path, so search in other "stacks" as well
        // search in children as well
        theItemElementToTheId = this->_getItemHolderWidgetById(id);
    }

    if(theItemElementToTheId == NULL) {
        WLog("Item not found. Cannot perform remove.");
        return;
    }

    theItemElementToTheId->hideAndDeleteAfterAnimated(true);
//    this->removeThisItemBecauseItWillDeleteItself(theItemElementToTheId); // the item will emit a signal which is connected to this method when it starts to delete itself, so it's unnecessary to remove it "manually"

#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog(" ?? before activate");
#endif

    if(isItemFoundInActiveItemPath) {
        activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, theNewPathItemRemoved);
    }
}

QList<MenuItemHolderWidgetBase *> MenuManager::getItemsUntilThisId(QString id, bool *outIsFound)
{
    QList<MenuItemHolderWidgetBase *> ret;

    outIsFound = false;
    return ret;
}

void MenuManager::_removeOrDeactivateItemFromNavigationPathById(QString id, bool isAlsoRemoveIt)
{
    // !!!
    // probably this should also check the child-items as well!

    QList<MenuItemHolderWidgetBase *> theNewPathItemRemoved;

    MenuItemHolderWidgetBase *theItemElementToTheId = NULL;

    int currItemPathCnt = currentActiveItemElementPath.size();
    for(int i = 0; i < currItemPathCnt && theItemElementToTheId == NULL; i++)
    {
        if(currentActiveItemElementPath[i]->getRelatedMenuItem()->getId() == id)
        {
            theItemElementToTheId = currentActiveItemElementPath[i];
        }
        else
        {
            theNewPathItemRemoved << currentActiveItemElementPath[i];
        }
    }

    if(theItemElementToTheId == NULL)
    {
        WLog("Item not found in current navigation path. Cannot perform remove.");
        return;
    }

    if(isAlsoRemoveIt)
    {
        theItemElementToTheId->hideAndDeleteAfterAnimated(true);
//    this->removeThisItemBecauseItWillDeleteItself(theItemElementToTheId); // the item will emit a signal which is connected to this method when it starts to delete itself, so it's unnecessary to remove it "manually"
    }

    activateCurrentItemPathBySnapshotChange(currentActiveItemElementPath, theNewPathItemRemoved);
}

MenuItemWidget *MenuManager::getItemById(QString id) {
    MenuItemHolderWidgetBase *holderWidget = this->_getItemHolderWidgetById(id);
    if(holderWidget != NULL) {
        return holderWidget->getRelatedMenuItem();
    }

    return NULL;
}

QWidget *MenuManager::getCustomWidgetOfItemByItemId(QString id)
{
    MenuItemHolderWidgetBase *holderWidget = this->_getItemHolderWidgetById(id);
    if(holderWidget != NULL) {
        return holderWidget->getCustomWidget();
    }

    return NULL;
}

MenuItemHolderWidgetBase *MenuManager::_getItemHolderWidgetById(QString id)
{
    if(id.isEmpty())
    {
        return NULL;
    }

    MenuItemHolderWidgetBase *ret = NULL;
    int cnt = rootMenuItems.size();
    for(int i = 0; i < cnt; i++)
    {
        ret = _getItemHolderWidgetById_Recursive(rootMenuItems[i], id);
        if(ret != NULL)
        {
            return ret;
        }
    }

    // not found
    return NULL;
}

MenuItemHolderWidgetBase *MenuManager::_getItemHolderWidgetById_Recursive(MenuItemHolderWidgetBase *head, QString id)
{
    MenuItemHolderWidgetBase *ret = NULL;
    if(head->getRelatedMenuItem()->getId() == id)
    {
        return head;
    } else {
        QList<MenuItemHolderWidgetBase *> tmpChildren = head->getChildrenElements();
        int cnt = tmpChildren.size();
        for(int i = 0; i < cnt; i++)
        {
            ret = _getItemHolderWidgetById_Recursive(tmpChildren[i], id);
            if(ret != NULL)
            {
                return ret;
            }
        }
    }

    return NULL;
}

MenuItemHolderWidgetBase *MenuManager::getItemElementByItem(MenuItemWidget *itemToSearchFor)
{
    MenuItemHolderWidgetBase *ret = NULL;
    int cnt = rootMenuItems.size();
    for(int i = 0; i < cnt; i++)
    {
        ret = _getItemElementByItem_Recursive(rootMenuItems[i], itemToSearchFor);
        if(ret != NULL)
        {
            return ret;
        }
    }

    // not found
    return NULL;
}

MenuItemHolderWidgetBase *MenuManager::_getItemElementByItem_Recursive(MenuItemHolderWidgetBase *head, MenuItemWidget *itemToSearchFor)
{
    MenuItemHolderWidgetBase *ret = NULL;
    if(head->getRelatedMenuItem() == itemToSearchFor)
    {
        return head;
    } else {
        QList<MenuItemHolderWidgetBase *> tmpChildren = head->getChildrenElements();
        int cnt = tmpChildren.size();
        for(int i = 0; i < cnt; i++)
        {
            ret = _getItemElementByItem_Recursive(tmpChildren[i], itemToSearchFor);
            if(ret != NULL)
            {
                return ret;
            }
        }
    }

    return NULL;
}

//void MenuManager::__forceUpdateMenuPresenterWidget()
//{
////    DLog(" #menu Force udpate!");
//    _menuPresenterWidget->update();
//}

void MenuManager::printThisListOfMenuItemElements(QList<MenuItemHolderWidgetBase *> menuItemElementList)
{
    DLogS << " -- item-path:";
    int cnt = menuItemElementList.size();
    for(int i = 0; i < cnt; i++)
    {
        DLogS << menuItemElementList[i]->getRelatedMenuItem()->getId();
    }
    DLogS << " -- end";
}
