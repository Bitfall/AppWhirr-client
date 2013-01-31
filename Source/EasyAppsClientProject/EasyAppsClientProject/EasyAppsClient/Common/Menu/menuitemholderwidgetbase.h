#ifndef MENUITEMHOLDERWIDGETBASE_H
#define MENUITEMHOLDERWIDGETBASE_H

#include <QWidget>

class MenuItemWidget;

//
// If this object deleted it will delete the related MenuItem and every child-element as well.
//  (This is done automatically via QObject parenting)
class MenuItemHolderWidgetBase : public QWidget
{
    Q_OBJECT
public:
    explicit MenuItemHolderWidgetBase(QWidget *parent = 0);

    virtual void addChildToLayout(MenuItemHolderWidgetBase *child) = 0;
    virtual QList<MenuItemHolderWidgetBase *> getChildrenElements() = 0;
    virtual void addCustomWidget(QWidget *customWidget) = 0;
    virtual QWidget *getCustomWidget() const = 0;
    virtual MenuItemWidget *getRelatedMenuItem() = 0;
    virtual void showImmediatelly(bool isTerminateShowHideAnimation = true) = 0;
    virtual void hideImmediatelly() = 0;
    virtual void _showAnimated() = 0;
    //
    // isUnstoppable: if true, then a show after this hide won't stop it in hideing and deleting itself
    //  use it carefully! it will delete the item immediatelly from the collections (through Q_SIGNAL/Q_SLOT)!
    virtual void hideAndDeleteAfterAnimated(bool isUnstoppable = false) = 0;
    // this one will only hide the item
    virtual void hideAnimated(bool isUnstoppable = false) = 0;
    virtual void activateWithLevelAnimated(int level) = 0;
    virtual void deactivateAnimated() = 0;
    virtual bool getIsDestroyingItself() const = 0;
    virtual bool getIsHideAnimationProposed() const = 0;
    virtual void setIsHideAnimationProposed(bool value) = 0;

Q_SIGNALS:
    void _startToShowSignal();
    void _startToHideSignal();
    //
    // have to emit this before/when the item starts to delete itself, e.g. when it's hideAndDeleteAfterAnimated method called
    void willDeleteItselfSignal(MenuItemHolderWidgetBase *itemElem);

//    void _itemForceUpdated();
};

#endif // MENUITEMHOLDERWIDGETBASE_H
