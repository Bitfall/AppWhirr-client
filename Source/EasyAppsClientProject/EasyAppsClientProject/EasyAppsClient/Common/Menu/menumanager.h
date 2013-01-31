#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QObject>

class MenuPresenterWidgetBase;
class MenuItemWidget;
class MenuItemHolderWidgetBase;

//
// Important notes:
//  * The menu system is highly dynamic and decenterized: the items can be removed anytime. To be more secure use the ID based methods which will do nothing if the item is removed from the menu-manager, or will return NULL if you try to get an item
//  * Removing an item will remove it immediatelly from the menu-manager, but it will only trigger a hide action on the view, and it will removed from the presenter only after the hide action / animation is finished
//  * Menu-manager is designed to use minimal memory, and won't cache child-items, only the root ones. So when you want to present a menu-item as a child / sub item you have to add it, and when you don't need it anymore you have to remove it. This will keep the memory usage low.
//  * An item will notify the menu-manager when it deletes itself, so you don't have to remove it "manually" from the item-collection
class MenuManager : public QObject
{
    Q_OBJECT
public:
    explicit MenuManager(MenuPresenterWidgetBase *menuPresenterWidget, QObject *parent = 0);

    //
    // note: adding an item with this will present it immediatelly
    void addRootItem(MenuItemWidget *item);
    //
    // note: adding an item (or custom widget) will present it immediatelly - you can control whether it shows animated or not with the parameter isAddItWithAnimation
    bool addChildToItem(MenuItemWidget *newChildItem, QString theParentItemId, QWidget *customWidgetToThisItem = NULL, bool isAddItWithAnimation = true);
    void addCustomWidgetToItem(QWidget *customWidget, QString itemId);

    MenuItemWidget *_getCurrentTopActiveItem();
    MenuItemWidget *getItemById(QString id);
    QWidget *getCustomWidgetOfItemByItemId(QString id);

    //
    // if isOnlyHighlightIt = false then the menu will be activated like it was clicked / activated by user action
    //  if it's true (default), then it will only highlight itself, and won't send the clicked related SIGNAL(s)
    void highlightItemById(QString id);
    //
    // this only deactivates the item and steps one step above in menu-hierarchy but doesn't hide the item
//    void deactivateItemInNavigationPathById(QString id);
    //
    // this deactivates and hides/removes the item from menu-hierarchy as well
    // note: it will remove the item from the navigation-manager immediatelly (if you try to get it via ID you will get NULL), but it won't remove it immediatelly from the presenter, but will start the hide animation, and when it ends it will remove it from the presenter
//    void removeItemInNavigationPathById(QString id);

    //
    // this deactivates and removes the item as well if found and visible
    // note: it will remove the item from the navigation-manager immediatelly (if you try to get it via ID you will get NULL), but it won't remove it immediatelly from the presenter, but will start the hide animation, and when it ends it will remove it from the presenter
    void removeItemById(QString id);

public Q_SLOTS:
    //
    // if isOnlyHighlightIt = false then the menu will be activated like it was clicked / activated by user action
    //  if it's true (default), then it will only highlight itself, and won't send the clicked related SIGNAL(s)
    void activateItemById(QString sourceMenuItemId);

private Q_SLOTS:
    //
    // only removes the item from the collection / stack, won't delete it
    void removeThisItemBecauseItWillDeleteItself(MenuItemHolderWidgetBase *itemElem);

//    void __forceUpdateMenuPresenterWidget();

private:
    MenuItemHolderWidgetBase *_getItemHolderWidgetById(QString id);
    void _activateItemById(QString sourceMenuItemId, bool isOnlyHighlightIt = false);

    //
    // note: adding an item with this will present it immediatelly - you can control whether it shows it animated or not with the parameter isAddItWithAnimation
    bool __addChildToItem(MenuItemWidget *newChildItem, MenuItemWidget *theParentItem, QWidget *customWidgetToThisItem = NULL, bool isAddItWithAnimation = true);
    //
    void _addCustomWidgetToItem(QWidget *customWidget, MenuItemWidget *item);

    MenuItemHolderWidgetBase *getItemElementByItem(MenuItemWidget *itemToSearchFor);
    MenuItemHolderWidgetBase *_getItemElementByItem_Recursive(MenuItemHolderWidgetBase *head, MenuItemWidget *itemToSearchFor);

    MenuItemHolderWidgetBase *_getItemHolderWidgetById_Recursive(MenuItemHolderWidgetBase *head, QString id);

    // return: -1 = not found; else = the index
    int getIndexInCurrentActiveItemPath(MenuItemWidget *item);

    void activateCurrentItemPathToThisIndex(int index);
    void activateThisRootItem(MenuItemHolderWidgetBase *itemElement);
    void addThisItemElementToActiveItemElementPathAndActivate(MenuItemHolderWidgetBase *itemElement);
    void stepBackToThisIndexInActiveItemPathAndAddAndActivateThisItemElement(int index, MenuItemHolderWidgetBase *itemElement);

    void _removeOrDeactivateItemFromNavigationPathById(QString id, bool isAlsoRemoveIt);
    void _removeItemById(QString id);
    QList<MenuItemHolderWidgetBase *> getItemsUntilThisId(QString id, bool *outIsFound);

    //
    // returns true if the given head is the one have to be removed
    //  -> an item cannot remove itself from it's parent
    //  so root items will be never removed. If it's a problem it has to be refactored. Anyway in this case if you call this method it will return true, indicating it found the item and it's a root item, and you can delete the whole stack if you want.
    // In any other cases it will return false
    bool _removeItem_Recursive(MenuItemHolderWidgetBase *head, QString id);

    //
    void activateCurrentItemPathBySnapshotChange(QList<MenuItemHolderWidgetBase *> sourceSnapshot, QList<MenuItemHolderWidgetBase *> targetSnapshot);

    void printThisListOfMenuItemElements(QList<MenuItemHolderWidgetBase *> menuItemElementList);

    //
    // return true if found, false if not
    bool getPathToThisItem(MenuItemHolderWidgetBase *head, MenuItemWidget *itemToSearchFor, QList<MenuItemHolderWidgetBase *> &refPath);

private:
    QList<MenuItemHolderWidgetBase *> rootMenuItems;
    // path of the items
    //  e.g.: selected Browser, then Web -> currentActiveItemPath will be { BrowserItemElement, WebItemElement }
    QList<MenuItemHolderWidgetBase *> currentActiveItemElementPath;

    MenuPresenterWidgetBase *_menuPresenterWidget;
};

#endif // MENUMANAGER_H
