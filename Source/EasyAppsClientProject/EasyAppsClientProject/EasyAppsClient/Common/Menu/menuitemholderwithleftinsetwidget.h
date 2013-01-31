#ifndef MENUITEMHOLDERWITHLEFTINSETWIDGET_H
#define MENUITEMHOLDERWITHLEFTINSETWIDGET_H

#include "menuitemholderwidgetbase.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPropertyAnimation;
class QParallelAnimationGroup;
QT_END_NAMESPACE

class MenuItemWidget;


//
class MenuItemHolderWithLeftInsetWidget : public MenuItemHolderWidgetBase
{
    Q_OBJECT

    Q_PROPERTY (float backgroundHighlightColorRate READ getBackgroundHighlightColorRate WRITE setBackgroundHighlightColorRate)
    Q_PROPERTY (float leftInset READ getLeftInset WRITE setLeftInset)
public:
    explicit MenuItemHolderWithLeftInsetWidget(MenuItemWidget *item, QColor inactiveBackgroundColor, QColor highlightedBackgroundBaseColor, QColor leftInsetColor, QWidget *parent = 0);

    void addChildToLayout(MenuItemHolderWidgetBase *child);
    QList<MenuItemHolderWidgetBase *> getChildrenElements();

    void addCustomWidget(QWidget *customWidget);
    QWidget *getCustomWidget() const;

    MenuItemWidget *getRelatedMenuItem();

    void showImmediatelly(bool isTerminateShowHideAnimation = true);
    void hideImmediatelly();

    void _showAnimated();
    //
    // isUnstoppable: if true, then a show after this hide won't stop it in hideing and deleting itself
    //  use it carefully! it will delete the item immediatelly from the collections (through Q_SIGNAL/Q_SLOT)!
    void hideAndDeleteAfterAnimated(bool isUnstoppable = false);
    void hideAnimated(bool isUnstoppable = false);
    void activateWithLevelAnimated(int level);
    void deactivateAnimated();

    bool getIsDestroyingItself() const;

    bool getIsHideAnimationProposed() const;
    void setIsHideAnimationProposed(bool value);

private Q_SLOTS:
    void showAnimationEnded();
    void hideAnimationEnded_OnlyHideItself();
    void hideAnimationEnded_DeleteItselfNow();
    void activateAnimationEnded();

    void _removeThisChild(MenuItemHolderWidgetBase *child);

protected:
    void paintEvent(QPaintEvent *e);
    bool event(QEvent *e);

private:
    void terminateShowHideAnimation();

private:

    void _willDeleteItself();

    void addActivateAnimationsToGroupAndStartItWithRefreshTimer(QList<QPropertyAnimation *> animList);
    QPropertyAnimation *createActivateBackgroundColorAnimation(float startRate, float endRate, int duration);
    QPropertyAnimation *createActivateLeftInsetAnimation(float startRate, float endRate, int duration);

    void _doHideAnimated();
    void _doHideAnimatedThenDeleteItself();
    void _doHideAndOptionallyDeleteAfter(bool isDeleteAfter);

    //
    // properties
    float getBackgroundHighlightColorRate() const;
    void setBackgroundHighlightColorRate(float value);

    float getLeftInset() const;
    void setLeftInset(float value);

private:
    QColor _highlightedBackgroundBaseColor;
    QColor _inactiveBackgroundColor;
    QColor _leftInsetColor;
    float _backgroundHighlightColorRate;
    float _leftInset;

    QVBoxLayout *_mainLayout;
    QVBoxLayout *_customWidgetHolderLayout;
    QWidget *_customWidget;

    QTimer *forceRefreshTimer;
    QParallelAnimationGroup *_showHideAnimationGroup;
    QParallelAnimationGroup *_activationAnimationGroup;
    bool _isHideAnimationProposed;

    bool _isDestroyingItself;

    MenuItemWidget *_menuItem;
    QList<MenuItemHolderWidgetBase *> _childElements;
};

#endif // MENUITEMHOLDERWITHLEFTINSETWIDGET_H
