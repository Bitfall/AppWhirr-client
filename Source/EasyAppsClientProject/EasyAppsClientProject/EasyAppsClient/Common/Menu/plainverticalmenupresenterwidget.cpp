#include "plainverticalmenupresenterwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSpacerItem>
#include <QResizeEvent>

#include <QDebug>

#include "menuitemwidget.h"
#include "menuitemholderwithleftinsetwidget.h"
#include "Common/Widgets/delegatingwidget.h"

// config
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

PlainVerticalMenuPresenterWidget::PlainVerticalMenuPresenterWidget(QWidget *parent) :
    MenuPresenterWidgetBase(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setContentsMargins(0, 0, 0, 0);

    _menuContentScrollArea = new QScrollArea;
    _menuContentScrollArea->setContentsMargins(0, 0, 0, 0);
    _menuContentScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _menuContentScrollArea->setWidgetResizable(false);
    _menuContentScrollArea->setFrameShape(QFrame::NoFrame);


    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

//    QString scrollAreaStyleSheet = GuiConfigurations::browserScrollerStyleSheet() + QString("QScrollArea { background-color: transparent; }");
    QString scrollAreaStyleSheet = guiConfManager->getStyleForId("ScrollArea/Dynamic/Default/Transparent/style");

    _menuContentScrollArea->setStyleSheet(scrollAreaStyleSheet);
//    menuContentScrollArea->setAttribute(Qt::WA_NoSystemBackground, true);
    {
        _menuContentHolderWidget = new DelegatingWidget;
        connect(_menuContentHolderWidget, SIGNAL(resizeHappened(QResizeEvent*)), this, SLOT(_menuContentHolderWidgetResized(QResizeEvent*)));
        _menuContentHolderWidget->setContentsMargins(0, 0, 0, 0);
        _menuContentLayout = new QVBoxLayout;
        _menuContentLayout->setContentsMargins(0, 0, 0, 0);
        _menuContentLayout->setSpacing(5);
        _menuContentHolderWidget->setLayout(_menuContentLayout);
        _menuContentHolderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
//        _menuContentLayout->setSizeConstraint(QLayout::SetFixedSize); // this sizes the menu correctly but it introduces some "flickering"
        _menuContentLayout->setSizeConstraint(QLayout::SetFixedSize); // this not so significant flickering, but it keeps growing - doesn't shrink to the correct minimal size
//        mainLayout->addWidget(menuContentHolderWidget);
        _menuContentHolderWidget->setFixedWidth(guiConfManager->getIntForId("MainMenu/fixedWidth"));

        _menuContentHolderWidget->setObjectName("menuContentHolderWidget");
//        menuContentHolderWidget->setStyleSheet("QWidget#menuContentHolderWidget { background-color: rgb(230, 230, 30); }");
        _menuContentHolderWidget->setStyleSheet("QWidget#menuContentHolderWidget { background-color: transparent; }");
//        this->setAttribute(Qt::WA_NoSystemBackground, true);

        _menuContentScrollArea->setWidget(_menuContentHolderWidget);
    }
    mainLayout->addWidget(_menuContentScrollArea);

//    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
//    mainLayout->addSpacerItem(spacerItem);

//    QWidget *spacerWidget = new QWidget;
//    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    mainLayout->addWidget(spacerWidget, 0);
}

void PlainVerticalMenuPresenterWidget::resizeEvent(QResizeEvent *e)
{
//    _menuContentHolderWidget->adjustSize();

//    DLog(" -- size hint: ") << _menuContentHolderWidget->sizeHint();
//    DLog(" -- log1: ") << _menuContentScrollArea->geometry();
//    DLog(" -- log2: ") << this->geometry();

    MenuPresenterWidgetBase::resizeEvent(e);
}

void PlainVerticalMenuPresenterWidget::_menuContentHolderWidgetResized(QResizeEvent *e)
{
//    DLog(" -- size hint: ") << this->_menuContentHolderWidget->sizeHint();
//    DLog(" -- log1: ") << _menuContentScrollArea->geometry();
//    DLog(" -- log2: ") << this->geometry();

//    _menuContentHolderWidget->adjustSize();

//    _menuContentScrollArea->resize(e->size());
}

void PlainVerticalMenuPresenterWidget::addRootMenuItemHolderWidget(MenuItemHolderWidgetBase *menuItemHolderWidget)
{
    this->_menuContentLayout->addWidget(menuItemHolderWidget);

//    _menuContentHolderWidget->adjustSize();
//    _menuContentScrollArea->resize(this->_menuContentHolderWidget->sizeHint());

//    DLog(" -- size hint: ") << this->_menuContentHolderWidget->sizeHint();
//    DLog(" -- log1: ") << _menuContentScrollArea->geometry();
//    DLog(" -- log2: ") << this->geometry();
}

MenuItemHolderWidgetBase *PlainVerticalMenuPresenterWidget::createMenuItemHolderWidgetByMenuItemWidget(MenuItemWidget *menuItemWidget)
{
    return new MenuItemHolderWithLeftInsetWidget(menuItemWidget, QColor(230, 230, 230), QColor(220, 220, 220), QColor(96, 174, 0));
}
