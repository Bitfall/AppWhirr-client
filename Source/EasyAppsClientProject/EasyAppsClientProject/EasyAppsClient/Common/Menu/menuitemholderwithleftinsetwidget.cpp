#include "menuitemholderwithleftinsetwidget.h"

#include <QVBoxLayout>

#include <QStateMachine>
#include <QState>
#include <QSignalTransition>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include <QPainter>
#include <QTimer>
#include <QApplication>

#include <QDebug>

#include "menuitemwidget.h"

// events
#include "proposehidemenuitemelementevent.h"
#include "proposehideanddeleteaftermenuitemelementevent.h"

// utility
#include <math.h>


// config
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

#define ANIMATION_DURATION 300
#define MAX_LEFT_INSET 12.0f
#define SPACING_BETWEEN_ITEMS 0


MenuItemHolderWithLeftInsetWidget::MenuItemHolderWithLeftInsetWidget(MenuItemWidget *item, QColor inactiveBackgroundColor, QColor highlightedBackgroundBaseColor, QColor leftInsetColor, QWidget *parent) :
    MenuItemHolderWidgetBase(parent),
    _highlightedBackgroundBaseColor(highlightedBackgroundBaseColor),
    _inactiveBackgroundColor(inactiveBackgroundColor),
    _leftInsetColor(leftInsetColor),
    _backgroundHighlightColorRate(0),
    _leftInset(0),
    _customWidget(NULL),
    _isHideAnimationProposed(false),
    _isDestroyingItself(false),
    _menuItem(item)
{
    // this one is not working, so make it "manually"
    //connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(_willDeleteSlot(QObject*)));

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    this->setContentsMargins(0, 0, 0, 0);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setFixedWidth(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getIntForId("MainMenu/fixedWidth"));

    // main layout
    _mainLayout = new QVBoxLayout;
    _mainLayout->setContentsMargins(this->_leftInset, 0, 0, 0);
    _mainLayout->setSpacing(SPACING_BETWEEN_ITEMS);
    this->setLayout(_mainLayout);

    // add the menu item
    _mainLayout->addWidget(_menuItem);

    // custom widget area and layout
    QWidget *customWidgetHolderWidget = new QWidget;
    customWidgetHolderWidget->setContentsMargins(0, 0, 0, 0);

    _customWidgetHolderLayout = new QVBoxLayout;
    _customWidgetHolderLayout->setContentsMargins(0, 0, 0, 0);
    _customWidgetHolderLayout->setSpacing(SPACING_BETWEEN_ITEMS);
    customWidgetHolderWidget->setLayout(_customWidgetHolderLayout);

    _mainLayout->addWidget(customWidgetHolderWidget);


    forceRefreshTimer = new QTimer(this);
    forceRefreshTimer->setInterval(33); // about 30 FPS
    connect(forceRefreshTimer, SIGNAL(timeout()), this, SLOT(update()));
//    connect(forceRefreshTimer, SIGNAL(timeout()), this, SIGNAL(_itemForceUpdated()));
    forceRefreshTimer->stop();

    _showHideAnimationGroup = new QParallelAnimationGroup(this);
    _activationAnimationGroup = new QParallelAnimationGroup(this);
}

void MenuItemHolderWithLeftInsetWidget::_willDeleteItself()
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("_willDeleteSlot");
#endif
    Q_EMIT willDeleteItselfSignal(this);
}

void MenuItemHolderWithLeftInsetWidget::addChildToLayout(MenuItemHolderWidgetBase *child)
{
    connect(child, SIGNAL(willDeleteItselfSignal(MenuItemHolderWidgetBase*)), this, SLOT(_removeThisChild(MenuItemHolderWidgetBase*)));
    _childElements << child;
    _mainLayout->addWidget(child);
}

void MenuItemHolderWithLeftInsetWidget::_removeThisChild(MenuItemHolderWidgetBase *child)
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("--rmv");
#endif
    if(child != NULL)
    {
#ifdef ENABLE_MENU_OPERATION_LOGGING
        DLog("remove child: ") << child->getRelatedMenuItem()->getId();
#endif
        child->disconnect(this, SLOT(_removeThisChild(MenuItemHolderWidgetBase*)));
        _childElements.removeAll(child);
    }
}

QList<MenuItemHolderWidgetBase *> MenuItemHolderWithLeftInsetWidget::getChildrenElements()
{
    return _childElements;
}

void MenuItemHolderWithLeftInsetWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
//    painter.setOpacity(backgroundColorRate);
    QColor currBackgroundColor(
                floor(this->_highlightedBackgroundBaseColor.red() * this->_backgroundHighlightColorRate) + ceil(this->_inactiveBackgroundColor.red() * (1.0f - this->_backgroundHighlightColorRate)),
                floor(this->_highlightedBackgroundBaseColor.green() * this->_backgroundHighlightColorRate) + ceil(this->_inactiveBackgroundColor.green() * (1.0f - this->_backgroundHighlightColorRate)),
                floor(this->_highlightedBackgroundBaseColor.blue() * this->_backgroundHighlightColorRate) + ceil(this->_inactiveBackgroundColor.blue() * (1.0f - this->_backgroundHighlightColorRate)),
                150 * this->_backgroundHighlightColorRate); // alpha

//    currBackgroundColor = QColor(200, 0, 0, 150);

    if(this->getLeftInset() > 0) {
        QRect fullRect = this->rect();
        QRect leftMarginRect(0, 0, this->getLeftInset(), fullRect.height());
        QRect theContentRect(this->getLeftInset(), 0, fullRect.width() - this->getLeftInset(), fullRect.height());

        painter.fillRect(leftMarginRect, this->_leftInsetColor);
        painter.fillRect(theContentRect, currBackgroundColor);
    }

    else {
        painter.fillRect(rect(), currBackgroundColor);
    }
}

void MenuItemHolderWithLeftInsetWidget::showImmediatelly(bool isTerminateShowHideAnimation)
{
    if(isTerminateShowHideAnimation) {
        _showHideAnimationGroup->stop();
    }

    this->setMinimumSize(this->sizeHint());
    this->show();
    this->update();
}

void MenuItemHolderWithLeftInsetWidget::hideImmediatelly()
{
    //this->setMinimumSize(0);
    this->hide();
    //hideAndDeleteAfterAnimated();
}

void MenuItemHolderWithLeftInsetWidget::_showAnimated()
{
#if 0
    if(isHideAnimationProposed)
    {
        //  this is a kinda workaround to allow presenting
        //  a previously-in-the-index-path item which is now
        //  deactivated. Without this the item would hide itself or would stuck in bigger size.
        this->animatedDeactivate();
    }
#endif

    this->setIsHideAnimationProposed(false);
#if 1
    if(this->isVisible())
    {
        // already shown, do nothing
        return;
    }

    // stop current animation
    _showHideAnimationGroup->stop();
    // and also clear it
    _showHideAnimationGroup->clear();

    // now add the new animations

    QPropertyAnimation *showAnimation1 = new QPropertyAnimation(this, "minimumSize");
    showAnimation1->setDuration(ANIMATION_DURATION);
    showAnimation1->setEasingCurve(QEasingCurve::OutExpo);
    showAnimation1->setStartValue(QSize(0, 0));
    showAnimation1->setEndValue(this->sizeHint());

    QPropertyAnimation *showAnimation2 = new QPropertyAnimation(this, "maximumSize");
    showAnimation2->setDuration(ANIMATION_DURATION);
    showAnimation2->setEasingCurve(QEasingCurve::OutExpo);
    showAnimation2->setStartValue(QSize(0, 0));
    showAnimation2->setEndValue(this->sizeHint());

    _showHideAnimationGroup->addAnimation(showAnimation1);
    _showHideAnimationGroup->addAnimation(showAnimation2);

    // disconnect every connection
    _showHideAnimationGroup->disconnect();
    // and connect the new one
    connect(_showHideAnimationGroup, SIGNAL(finished()), this, SLOT(showAnimationEnded()));
    _showHideAnimationGroup->start();
#else
    Q_EMIT _startToShowSignal();
#endif

    this->show();
//    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    //forceRefreshTimer->start();
}

void MenuItemHolderWithLeftInsetWidget::hideAnimated(bool isUnstoppable)
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("hide and delete");
#endif

    if(!isUnstoppable)
    {
        this->setIsHideAnimationProposed(true);
        QApplication::postEvent(this, new ProposeHideMenuItemElementEvent());
    }
    else
    {
#ifdef ENABLE_MENU_OPERATION_LOGGING
        DLog("hide: unstoppable flag is set!");
#endif
        this->_doHideAnimated();
    }
}

void MenuItemHolderWithLeftInsetWidget::hideAndDeleteAfterAnimated(bool isUnstoppable)
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("hide and delete");
#endif

    if(!isUnstoppable)
    {
        this->setIsHideAnimationProposed(true);
        QApplication::postEvent(this, new ProposeHideAndDeleteAfterMenuItemElementEvent());
    }
    else
    {
#ifdef ENABLE_MENU_OPERATION_LOGGING
        DLog("hide: unstoppable flag is set!");
#endif
        this->_doHideAnimatedThenDeleteItself();
    }
}

bool MenuItemHolderWithLeftInsetWidget::event(QEvent *e)
{
    if(e->type() == ProposeHideMenuItemElementEvent::getCustomEventType())
    {
        if(this->getIsHideAnimationProposed())
        {
            _doHideAnimated();
        }

        return true;
    }

    else if(e->type() == ProposeHideAndDeleteAfterMenuItemElementEvent::getCustomEventType())
    {
        if(this->getIsHideAnimationProposed())
        {
            _doHideAnimatedThenDeleteItself();
        }

        return true;
    }

    return QWidget::event(e);
}

void MenuItemHolderWithLeftInsetWidget::_doHideAnimated()
{
    this->_doHideAndOptionallyDeleteAfter(false);
}

void MenuItemHolderWithLeftInsetWidget::_doHideAnimatedThenDeleteItself()
{
    this->_doHideAndOptionallyDeleteAfter(true);
}

void MenuItemHolderWithLeftInsetWidget::_doHideAndOptionallyDeleteAfter(bool isDeleteAfter)
{
    this->setIsHideAnimationProposed(false);

    if(!this->isVisible() && !isDeleteAfter)
    {
        // already hidden, do nothing
        return;
    }

    if(isDeleteAfter) {
        _isDestroyingItself = true;
        _willDeleteItself();
    }

    // stop current animation
    _showHideAnimationGroup->stop();
    // and also clear it
    _showHideAnimationGroup->clear();
    // and disconnect
    _showHideAnimationGroup->disconnect();

    // now add the new animations

    QPropertyAnimation *showAnimation1 = new QPropertyAnimation(this, "minimumSize");
    showAnimation1->setDuration(ANIMATION_DURATION);
    showAnimation1->setEasingCurve(QEasingCurve::OutExpo);
    showAnimation1->setStartValue(this->size());
    showAnimation1->setEndValue(QSize(0, 0));

    QPropertyAnimation *showAnimation2 = new QPropertyAnimation(this, "maximumSize");
    showAnimation2->setDuration(ANIMATION_DURATION);
    showAnimation2->setEasingCurve(QEasingCurve::OutExpo);
    showAnimation2->setStartValue(this->size());
    showAnimation2->setEndValue(QSize(0, 0));

    _showHideAnimationGroup->addAnimation(showAnimation1);
    _showHideAnimationGroup->addAnimation(showAnimation2);

    // and connect the new one
    if(isDeleteAfter) {
        connect(_showHideAnimationGroup, SIGNAL(finished()), this, SLOT(hideAnimationEnded_DeleteItselfNow()));
    }
    else {
        connect(_showHideAnimationGroup, SIGNAL(finished()), this, SLOT(hideAnimationEnded_OnlyHideItself()));
    }
    _showHideAnimationGroup->start();

    //forceRefreshTimer->start();

    // and also deactivate it
    this->deactivateAnimated();
}

MenuItemWidget *MenuItemHolderWithLeftInsetWidget::getRelatedMenuItem()
{
    return _menuItem;
}

void MenuItemHolderWithLeftInsetWidget::deactivateAnimated()
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
        DLog("deactivateAnimated");
#endif
    QPropertyAnimation *backgroundColorAnim = createActivateBackgroundColorAnimation(this->_backgroundHighlightColorRate, 0, ANIMATION_DURATION);
    QPropertyAnimation *leftInsetAnim = this->createActivateLeftInsetAnimation(this->getLeftInset(), 0, ANIMATION_DURATION);

    QList<QPropertyAnimation *> activateAnims;
    activateAnims << backgroundColorAnim;
    activateAnims << leftInsetAnim;

    this->addActivateAnimationsToGroupAndStartItWithRefreshTimer(activateAnims);
//    connect(activateAnimation, SIGNAL(finished()), this, SLOT(activateAnimationEnded()));

//    forceRefreshTimer->start();

//    activateAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    // and also hide it's children
    int childCnt = this->_childElements.size();
    for(int i = 0; i < childCnt; i++)
    {
#ifdef ENABLE_MENU_OPERATION_LOGGING
        DLog("xxx child");
#endif
//        childrenElements[i]->hideAndDeleteAfterAnimated(false);
        _childElements[i]->hideAnimated(false);
    }

    _menuItem->_deactivateAnimated();
}

void MenuItemHolderWithLeftInsetWidget::terminateShowHideAnimation()
{
    _showHideAnimationGroup->stop();
}

void MenuItemHolderWithLeftInsetWidget::activateWithLevelAnimated(int level)
{
    QPropertyAnimation *backgroundColorAnim = NULL;
    QPropertyAnimation *leftInsetAnim = NULL;
    if(level == 0) {
        // top-level
        backgroundColorAnim = this->createActivateBackgroundColorAnimation(this->_backgroundHighlightColorRate, 1.0f, ANIMATION_DURATION);
        leftInsetAnim = this->createActivateLeftInsetAnimation(this->getLeftInset(), MAX_LEFT_INSET, ANIMATION_DURATION);
    } else {
        backgroundColorAnim = this->createActivateBackgroundColorAnimation(this->_backgroundHighlightColorRate, (0.7f / ((float)(level + 1))), ANIMATION_DURATION);
        leftInsetAnim = this->createActivateLeftInsetAnimation(this->getLeftInset(), (MAX_LEFT_INSET / ((float)(level + 1))), ANIMATION_DURATION);
    }

    QList<QPropertyAnimation *> activateAnims;
    activateAnims << backgroundColorAnim;
    activateAnims << leftInsetAnim;

    this->addActivateAnimationsToGroupAndStartItWithRefreshTimer(activateAnims);




    // and also present it's children
    int childCnt = this->_childElements.size();
    for(int i = 0; i < childCnt; i++)
    {
        //qDebug("show child");
#if 1 // moved
        if(_childElements[i]->getIsHideAnimationProposed()) {
            // terminate it
            //  this is a kinda workaround to allow presenting
            //  a previously-in-the-index-path item which is now
            //  deactivated. Without this the item would hide itself or would stuck in bigger size.
            _childElements[i]->deactivateAnimated();
            _childElements[i]->setIsHideAnimationProposed(false);
        }
#endif

        _childElements[i]->_showAnimated();
    }

    _menuItem->_activateAnimated();
}

QPropertyAnimation *MenuItemHolderWithLeftInsetWidget::createActivateBackgroundColorAnimation(float startRate, float endRate, int duration)
{
    QPropertyAnimation *activateAnimation = new QPropertyAnimation(this, "backgroundHighlightColorRate");
    activateAnimation->setDuration(duration);
    activateAnimation->setEasingCurve(QEasingCurve::Linear);
    activateAnimation->setStartValue(startRate);
    activateAnimation->setEndValue(endRate);

    return activateAnimation;
}

QPropertyAnimation *MenuItemHolderWithLeftInsetWidget::createActivateLeftInsetAnimation(float startRate, float endRate, int duration) {
    QPropertyAnimation *anim = new QPropertyAnimation(this, "leftInset");
    anim->setDuration(duration);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->setStartValue(startRate);
    anim->setEndValue(endRate);

    return anim;
}

void MenuItemHolderWithLeftInsetWidget::activateAnimationEnded()
{
    forceRefreshTimer->stop();
    // ensure final update
    this->update();
}

void MenuItemHolderWithLeftInsetWidget::showAnimationEnded()
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    qDebug("showAnimationEnded");
#endif

    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    //forceRefreshTimer->stop();
}

// slot
void MenuItemHolderWithLeftInsetWidget::hideAnimationEnded_OnlyHideItself()
{
#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("hideAnimationEnded");
#endif
    this->hide();
}

// slot
void MenuItemHolderWithLeftInsetWidget::hideAnimationEnded_DeleteItselfNow()
{
    //forceRefreshTimer->stop();

#ifdef ENABLE_MENU_OPERATION_LOGGING
    DLog("hideAnimationEnded");
#endif
    this->hide();
    //isHideAnimationProposed = false;
    //_willDeleteItself();
    this->setParent(NULL);
    this->deleteLater();
}

void MenuItemHolderWithLeftInsetWidget::addCustomWidget(QWidget *customWidget)
{
    if(customWidget == NULL) {
        WLog("The specified custom-widget is invalid (NULL)");
        return;
    }
    if(_customWidget == NULL) {
        _customWidgetHolderLayout->addWidget(customWidget);
    }
    else {
        _customWidgetHolderLayout->removeWidget(_customWidget);
        _customWidgetHolderLayout->addWidget(customWidget);
    }

    _customWidget = customWidget;
}

QWidget *MenuItemHolderWithLeftInsetWidget::getCustomWidget() const
{
    return this->_customWidget;
}

void MenuItemHolderWithLeftInsetWidget::addActivateAnimationsToGroupAndStartItWithRefreshTimer(QList<QPropertyAnimation *> animList)
{
    // stop and clear the previous one
    this->_activationAnimationGroup->stop();
    this->_activationAnimationGroup->disconnect();
    this->_activationAnimationGroup->clear();

    // add the new animations
    int animListCnt = animList.size();
    for(int i = 0; i < animListCnt; i++) {
        this->_activationAnimationGroup->addAnimation(animList[i]);
    }

    // connect
    connect(this->_activationAnimationGroup, SIGNAL(finished()), this, SLOT(activateAnimationEnded()));

    // start refresh times
    forceRefreshTimer->start();

    // and start the anim-group
    this->_activationAnimationGroup->start();
}

//
// properties
float MenuItemHolderWithLeftInsetWidget::getBackgroundHighlightColorRate() const
{
    return _backgroundHighlightColorRate;
}

void MenuItemHolderWithLeftInsetWidget::setBackgroundHighlightColorRate(float value)
{
    _backgroundHighlightColorRate = value;
}

bool MenuItemHolderWithLeftInsetWidget::getIsDestroyingItself() const
{
    return _isDestroyingItself;
}

bool MenuItemHolderWithLeftInsetWidget::getIsHideAnimationProposed() const {
    return this->_isHideAnimationProposed;
}

void MenuItemHolderWithLeftInsetWidget::setIsHideAnimationProposed(bool value) {
    this->_isHideAnimationProposed = value;
}

float MenuItemHolderWithLeftInsetWidget::getLeftInset() const
{
    return this->_leftInset;
}

void MenuItemHolderWithLeftInsetWidget::setLeftInset(float value)
{
    this->_leftInset = value;
    _mainLayout->setContentsMargins(this->_leftInset, 0, 0, 0);
}
