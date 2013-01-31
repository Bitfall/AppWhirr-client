#include "optionscontentpage.h"

// UI
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QSpacerItem>
#include <QLabel>

#include <QProcess>

#include "Common/Widgets/checkboxbutton.h"

#include <QSignalMapper>

#include "Common/Widgets/textandacceptcancelbuttonsnotificationwidget.h"

// menu
#include "Browser/browserwindow.h"
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
#include "Settings/menuitemidcenter.h"
// page navigation manager
#include "Common/Page/pagenavigationmanager.h"
#include "Settings/pageidcenter.h"
// global objects
#include "Application/globalobjectregistry.h"
#include "Browser/Notification/notificationmanager.h"
// sub-pages
#include "Browser/Content/Page/suggestanapppage.h"
#include "Browser/Content/Page/feedbackpage.h"
#include "Browser/Content/Page/whatsnewpage.h"
#include "Browser/Content/Page/appwhirrlicencepage.h"
#include "Browser/Content/Page/uninstallconfirmandfeedbackpage.h"

#include "Settings/settings.h"
#include "Application/appwhirrapplication.h"

#include "Common/Security/authorization.h"

// utility
#include "Application/Utils/awshortcuthandler.h"
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"

// config
#include "Settings/configmanager.h"
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

//#define AUTO_STARTUP_TEXT                                       "Auto startup"
//#define HIDE_LAUNCHER_RATHER_THAN_MINIMIZE_TEXT                 "Hide Launcher rather then minimize"
//#define USE_HOTKEY_FOR_LAUNCHER_TEXT                            "Use hotkeys for Launcher"
//#define SHOW_LAUNCHER_AT_STARTUP_TEXT                           "Show Launcher at startup"
//#define ADD_APPLICATION_TO_SYSTEM_TRAY_TEXT                     "Add application to system tray"
//#define SHOW_LAUNCHER_HIDE_SYSTEM_TRAY_MESSAGE_ONLY_ONCE_TEXT   "Show Launcher hide system tray message only once"

OptionsContentPage::OptionsContentPage(QString pageId, QWidget *parent) :
        PageWidgetBase(pageId, parent)
{

}

void OptionsContentPage::initializePageContent()
{
    signalMapper = new QSignalMapper(this);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    this->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));
    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);

    QWidget *optionsContentHolderWidget = new QWidget;
    QVBoxLayout *optionsContentHolderLayout = new QVBoxLayout;
    optionsContentHolderWidget->setLayout(optionsContentHolderLayout);
    optionsContentHolderWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    optionsContentHolderLayout->setContentsMargins(0, 0, 0, 0);
    optionsContentHolderLayout->setSpacing(5);

    // Init elements
//    QCheckBox* autoStartupCheckbox = new QCheckBox(AUTO_STARTUP_TEXT, this);
    _autoStartupCheckbox = new CheckboxButton(tr("Start AppWhirr automatically when my computer starts"));
    _autoStartupCheckbox->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
    _autoStartupCheckbox->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    connect(_autoStartupCheckbox, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(_autoStartupCheckbox, _autoStartupCheckbox);
    optionsContentHolderLayout->addWidget(_autoStartupCheckbox);

    _globalHotkeyToLauncherCheckbox = new CheckboxButton(tr("Enable hotkey (Ctrl + Shift + Space)"), this);
    _globalHotkeyToLauncherCheckbox->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
    _globalHotkeyToLauncherCheckbox->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    connect(_globalHotkeyToLauncherCheckbox, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(_globalHotkeyToLauncherCheckbox, _globalHotkeyToLauncherCheckbox);
    optionsContentHolderLayout->addWidget(_globalHotkeyToLauncherCheckbox);

//#ifdef ENABLE_DEVELOPER_PREVIEW_FEATURES
//    _showLauncherAtStartupCheckbox = new CheckboxButton(tr("Show Launcher at startup"), this);
//    _showLauncherAtStartupCheckbox->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
//    _showLauncherAtStartupCheckbox->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
//    connect(_showLauncherAtStartupCheckbox, SIGNAL(clicked()), signalMapper, SLOT(map()));
//    signalMapper->setMapping(_showLauncherAtStartupCheckbox, _showLauncherAtStartupCheckbox);
//    optionsContentHolderLayout->addWidget(_showLauncherAtStartupCheckbox);
//#endif

//#ifdef ENABLE_DEVELOPER_PREVIEW_FEATURES
//    QComboBox *preferedLanguageComboBox = new QComboBox;
//    preferedLanguageComboBox->addItem("English");
//    preferedLanguageComboBox->addItem("Magyar");
////    connect()
//    optionsContentHolderLayout->addWidget(preferedLanguageComboBox);
//#endif

    QPushButton *createDesktopShortcutButton = new QPushButton(tr("Create Desktop Shortcut for AppWhirr client"));
    createDesktopShortcutButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    createDesktopShortcutButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    createDesktopShortcutButton->setFlat(true);
    connect(createDesktopShortcutButton, SIGNAL(clicked()), this, SLOT(_createDesktopShortcutForClient()));
    optionsContentHolderLayout->addWidget(createDesktopShortcutButton);

//    QPushButton *switchUserButton = new QPushButton(tr("Switch User"));
//    switchUserButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
//    connect(switchUserButton, SIGNAL(clicked()), this, SLOT(switchUserButtonPressed()));
//    optionsContentHolderLayout->addWidget(switchUserButton);

#if 0
    QPushButton *deleteLocalDataButton = new QPushButton(tr(" (!) Delete AppWhirr's local data and installed apps"));
    deleteLocalDataButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    deleteLocalDataButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    deleteLocalDataButton->setFlat(true);
    connect(deleteLocalDataButton, SIGNAL(clicked()), this, SLOT(deleteLocalDataButtonPressed()));
    optionsContentHolderLayout->addWidget(deleteLocalDataButton);

    QPushButton *deleteCacheButton = new QPushButton(tr(" (!) Delete AppWhirr's cache"));
    deleteCacheButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    deleteCacheButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    deleteCacheButton->setFlat(true);
    connect(deleteCacheButton, SIGNAL(clicked()), this, SLOT(deleteCacheButtonPressed()));
    optionsContentHolderLayout->addWidget(deleteCacheButton);
#endif

#ifndef PUBLIC_PRODUCTION_READY_VERSION
    QLabel *versionNumberLabel = new QLabel(tr("Version number: %1").arg(ConfigManager::getClientVersion()));
    versionNumberLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    optionsContentHolderLayout->addWidget(versionNumberLabel);
#endif

    QPushButton *quitButton = new QPushButton(tr(" Quit now!"));
    quitButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    quitButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    quitButton->setFlat(true);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitButtonPressed()));
    optionsContentHolderLayout->addWidget(quitButton);

    QPushButton *logOffUserButton = new QPushButton(tr("Log out and restart AppWhirr"));
    logOffUserButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    logOffUserButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    logOffUserButton->setFlat(true);
    connect(logOffUserButton, SIGNAL(clicked()), this, SLOT(logOffUserButtonPressed()));
    optionsContentHolderLayout->addWidget(logOffUserButton);

    optionsContentHolderLayout->addSpacerItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QPushButton *uninstallAppWhirrButton = new QPushButton(tr("Uninstall AppWhirr and/or every app"));
    uninstallAppWhirrButton->setStyleSheet(guiConfManager->getStyleForId("Button/DangerousAction/style"));
    uninstallAppWhirrButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    uninstallAppWhirrButton->setFlat(true);
    connect(uninstallAppWhirrButton, SIGNAL(clicked()), this, SLOT(uninstallAppWhirrButtonClicked()));
    optionsContentHolderLayout->addWidget(uninstallAppWhirrButton);
    optionsContentHolderLayout->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));

    // Setup values
    _autoStartupCheckbox->setChecked(Settings::sharedSettings()->getIsAutoStartAtSystemStartup());
    _globalHotkeyToLauncherCheckbox->setChecked(Settings::sharedSettings()->getIsSetGlobalHotkeyToLauncher());
//    _showLauncherAtStartupCheckbox->setChecked(Settings::sharedSettings()->getIsShowLauncherAtStartup());

    connect(signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(checkBoxChanged(QWidget*)));

//    QWidget *sizerWidget = new QWidget;
//    optionsContentHolderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addSpacing(15);
    QHBoxLayout *optionsContentCenterizerLayout = new QHBoxLayout;
    optionsContentCenterizerLayout->addStretch();
    optionsContentCenterizerLayout->addWidget(optionsContentHolderWidget);
    optionsContentCenterizerLayout->addStretch();
    mainLayout->addLayout(optionsContentCenterizerLayout);
//    this->mainLayout->addWidget(optionsContentHolderWidget);

    this->mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
//    this->mainLayout->addWidget(sizerWidget);
//    this->mainLayout->addSpacing();
}

void OptionsContentPage::switchUserButtonPressed()
{
    Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
}

void OptionsContentPage::deleteLocalDataButtonPressed()
{
    TextAndAcceptCancelButtonsNotificationWidget *confirmDeleteLocalDataNotificationWidget = new TextAndAcceptCancelButtonsNotificationWidget(tr("Are you sure you want to delete every local AppWhirr information and app?"), tr("I'm sure, delete it"), tr("Don't delete"), true);
    connect(confirmDeleteLocalDataNotificationWidget, SIGNAL(acceptOptionSelected()), this, SLOT(_doDeleteLocalData()));
//    connect(confirmUninstallNotificationWidget, SIGNAL(cancelOptionSelected()), this, SLOT(_uninstallConfirmCanceled()));
//    confirmUninstallNotificationWidget->setOptionalId(appId);

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(confirmDeleteLocalDataNotificationWidget, "Are you sure you want to delete every local AppWhirr information and app?", "OptionsContentPage::deleteLocalData", true);
}

void OptionsContentPage::deleteCacheButtonPressed()
{
    if( PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getCacheDirectory()) ) {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultSuccessMessageNotificationWithTimeout(tr("Cache deleted."));
    }
    else {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultErrorMessageNotificationWithTimeout(tr("Cache data cannot be removed. You can remove it at: %1").arg(AppSpecificPathHelper::getCacheDirectory()));
        PathHelper::logLastOSSpecificFileSystemHandlingError();
    }
}

void OptionsContentPage::quitButtonPressed()
{
    qApp->quit();
}

void OptionsContentPage::logOffUserButtonPressed()
{
    // do not remember me
    Settings::sharedSettings()->setIsRememberLastUser(false);
    Settings::sharedSettings()->saveSettingsNow();

    // restart appwhirr
    QString appWhirrUpdaterModeCode = "3";
#ifdef Q_WS_WIN
    QStringList processParams;
    processParams << appWhirrUpdaterModeCode << AppSpecificPathHelper::getClientExecutablePath() << AppSpecificPathHelper::getClientExecutableDirectory();
    if (!QProcess::startDetached(AppSpecificPathHelper::getUpdaterExecutablePath(), processParams)) {
#endif
#ifdef Q_WS_MAC
            QStringList processParams;
            processParams << AppSpecificPathHelper::getUpdaterExecutablePath() << "--args" << appWhirrUpdaterModeCode << AppSpecificPathHelper::getClientExecutablePath() << AppSpecificPathHelper::getClientExecutableDirectory();
            DLog("InstallerProcessParams: ") << processParams;
            if (!QProcess::startDetached("open", processParams)) {
#endif
        WLog("Failed to start the logoff-restart, for client-path: " << AppSpecificPathHelper::getClientExecutableDirectory());
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultErrorMessageNotificationWithTimeout(tr("Cannot log you off because cannot restart AppWhirr"));
    }
    else {
                // close the client
                AppwhirrApplication::instance()->quit();
            }
}

void OptionsContentPage::_doDeleteLocalData()
{
    if( PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getLocalRootDirectory(), false) ) {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultSuccessMessageNotificationWithTimeout(tr("Local data deleted. Restart AppWhirr to apply the changes."));
    }
    else {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultErrorMessageNotificationWithTimeout(tr("Local data cannot be removed. You can remove it manually at: %1").arg(AppSpecificPathHelper::getLocalRootDirectory()));
        PathHelper::logLastOSSpecificFileSystemHandlingError();
    }
}

void OptionsContentPage::uninstallAppWhirrButtonClicked()
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    pageNavigationManager->pushPageToCurrentRootStackAndShow(new UninstallConfirmAndFeedbackPage("page-id-UninstallConfirmAndFeedbackPage"));
}

void OptionsContentPage::_createDesktopShortcutForClient()
{
    if(!AW::Client::Utils::AWShortcutHandler::createDesktopShortcutForAppWhirrClient())
    {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultErrorMessageNotificationWithTimeout(tr("Cannot create the Desctop Shortcut. Sorry."));
    } else {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultSuccessMessageNotificationWithTimeout(tr("Desktop Shortcut created."));
    }
}

void OptionsContentPage::pagePushWillHappen()
{
    // this is a root page, will be pushed only once
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    MenuItemWidget *menuItem = menuManager->getItemById(MenuItemIdCenter::optionsMenuId());

    if(menuItem != NULL && menuManager != NULL)
    {
        // suggest an app
        MenuItemWidget *suggestAnAppMenuItem = new MenuItemWidget(MenuItemIdCenter::suggestAnAppMenuId(), tr("Suggest an app"));
        connect(suggestAnAppMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(presentSuggestAnAppPage(QString)));

        menuManager->addChildToItem(suggestAnAppMenuItem, menuItem->getId(), NULL, true);

        // feedback
        MenuItemWidget *feedbackMenuItem = new MenuItemWidget(MenuItemIdCenter::feedbackPageMenuId(), tr("Send feedback"));
        connect(feedbackMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(presentFeedbackPage(QString)));
        menuManager->addChildToItem(feedbackMenuItem, menuItem->getId(), NULL, true);

        // what's new
        MenuItemWidget *whatsNewMenuItem = new MenuItemWidget(MenuItemIdCenter::whatsNewPageMenuId(), tr("What's new"));
        connect(whatsNewMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(presentWhatsNewPage(QString)));
        menuManager->addChildToItem(whatsNewMenuItem, menuItem->getId(), NULL, true);

        // what's new
        MenuItemWidget *appwhirrLicenceMenuItem = new MenuItemWidget(MenuItemIdCenter::appwhirrLicencePresenterPageMenuId(), tr("Terms of Service"));
        connect(appwhirrLicenceMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(presentAppwhirrLicenceMenuItem(QString)));
        menuManager->addChildToItem(appwhirrLicenceMenuItem, menuItem->getId(), NULL, true);
    }
    else
    {
        qDebug("The menu-item is NULL. This page requires a valid menu-item.");
    }
}

// Q_SLOT
void OptionsContentPage::checkBoxChanged(QWidget *value) {
    CheckboxButton *checkbox = qobject_cast<CheckboxButton *>(value);
    if (checkbox == 0) {
        WLog("Cannot cast!");
        return;
    }

    if (checkbox == _autoStartupCheckbox) {
        Settings::sharedSettings()->setIsAutoStartAtSystemStartup(checkbox->getIsChecked());
        // and also refresh the state via MyApplication
        AppwhirrApplication::instance()->actualizeAutoStartupStateBySettings();
    }

    else if (checkbox == _globalHotkeyToLauncherCheckbox) {
        bool isChecked = checkbox->getIsChecked();
        Settings::sharedSettings()->setIsSetGlobalHotkeyToLauncher(isChecked);
        if(isChecked) {
            AppwhirrApplication::instance()->registerGlobalHotkeyForBrowserWindow();
        }
        else {
            AppwhirrApplication::instance()->unregisterGlobalHotkeyForBrowserWindow();
        }
    }

//    else if (checkbox == _showLauncherAtStartupCheckbox) {
//        Settings::sharedSettings()->setIsShowLauncherAtStartup(checkbox->getIsChecked());
//    }

    else {
        WLog("No match found.");
    }
}

void OptionsContentPage::relatedMenuItemActivated(QString menuItemId)
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::optionsPageId())) {
        // this root page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(PageIdCenter::optionsPageId());
    } else {
        // currently this root page's stack is not the active one, so switch to it
        pageNavigationManager->switchToRootPageStack(PageIdCenter::optionsPageId());
    }
}

void OptionsContentPage::presentSuggestAnAppPage(QString menuItemId)
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
//    pageNavigationManager->popBackToThisPage(this);
//    pageNavigationManager->pushPageToCurrentRootStackAndShow(new SuggestAnAppPage("page-id-SuggestAnAppPage"));
    pageNavigationManager->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::optionsPageId(), new SuggestAnAppPage(PageIdCenter::suggestAnAppOptionsSubPageId()));
}

void OptionsContentPage::presentFeedbackPage(QString menuItemId)
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
//    pageNavigationManager->popBackToThisPage(this);
//    pageNavigationManager->pushPageToCurrentRootStackAndShow(new FeedbackPage("page-id-FeedbackPage"));
    pageNavigationManager->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::optionsPageId(), new FeedbackPage("page-id-FeedbackPage"));
}

void OptionsContentPage::presentWhatsNewPage(QString menuItemId)
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    pageNavigationManager->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::optionsPageId(), new WhatsNewPage("page-id-WhatsNewPage"));
}

void OptionsContentPage::presentAppwhirrLicenceMenuItem(QString menuItemId)
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    pageNavigationManager->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::optionsPageId(), new AppwhirrLicencePage("page-id-AppwhirrLicencePage"));
}

void OptionsContentPage::pageWillAppear(QString param)
{
    // select the related menu item
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::optionsMenuId());
}

void OptionsContentPage::pageDidAppear()
{
//    qDebug("page did appear");
}

void OptionsContentPage::pagePopWillHappen()
{
    // this is a root page, pop will never happen
}

void OptionsContentPage::pageWillDisappear()
{

}
