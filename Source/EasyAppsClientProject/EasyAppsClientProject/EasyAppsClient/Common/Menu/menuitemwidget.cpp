#include "menuitemwidget.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/globalobjectregistry.h"

#include "Common/Widgets/simpleimagepresenterwidget.h"
#include "Common/imagetypesenum.h"

MenuItemWidget::MenuItemWidget(QString id, QString title, int badgeNumber, QWidget *parent) :
    QWidget(parent),
    itemId(id)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);

    // Title
    itemTitleLabel = new QLabel;
    itemTitleLabel->setText(title);
    itemTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    itemTitleLabel->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("MainMenu/TextLabel/Inactive/style"));
    mainLayout->addWidget(itemTitleLabel);

    // Badge
    QWidget* badgeContainer = new QWidget;
    badgeContainer->setFixedWidth(24);
    QVBoxLayout* badgeLayout = new QVBoxLayout;
    badgeLayout->setContentsMargins(0,0,0,0);

//    badge = new SimpleImagePresenterWidget();
    badge = new QLabel();
    badge->setFixedSize(24, 36);
//    badge->setImageByPath(":/badge", ImageTypes::PixelBased);
    badge->setStyleSheet("QLabel { background-image:url(':/badge'); background-repeat: no-repeat; color:white;}");
    badge->setContentsMargins(0, 0, 0, 0);
    badge->setAlignment(Qt::AlignCenter);

    badgeContainer->setLayout(badgeLayout);
    badgeLayout->addWidget(badge, 0, Qt::AlignTop);

    mainLayout->addStretch(0);
    mainLayout->addWidget(badgeContainer);

    this->setAttribute(Qt::WA_NoSystemBackground, true);

    setBadgeNumber(badgeNumber);
}

void MenuItemWidget::_activateAnimated()
{
    itemTitleLabel->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("MainMenu/TextLabel/Active/style"));
}

void MenuItemWidget::_deactivateAnimated()
{
    itemTitleLabel->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("MainMenu/TextLabel/Inactive/style"));
}

void MenuItemWidget::_emitItemBecomeTheTopActiveOneSignal()
{
    Q_EMIT itemBecomeTheTopActiveOne(this->getId());
}

QString MenuItemWidget::getId() const
{
    return itemId;
}

void MenuItemWidget::setTitle(QString title)
{
    itemTitleLabel->setText(title);
}

void MenuItemWidget::setBadgeNumber(int number) {
    badgeNumber = number;
    badge->setText(QString("%1").arg(badgeNumber));
    badgeNumber > 0 ? badge->show() : badge->hide();
}

bool MenuItemWidget::event(QEvent *e)
{
#if 1
    if(e->type() == QEvent::MouseButtonPress)
    {
#ifdef ENABLE_MENU_OPERATION_LOGGING
        DLog("menu item clicked");
#endif
        Q_EMIT itemClick(this->getId());

        //this->showChildren();

        return true;
    }
#endif

    return QWidget::event(e);
}
