#include "globalobjectregistry.h"

#include "Common/Page/pagenavigationmanager.h"
#include "Common/Menu/menumanager.h"
#include "Browser/Notification/notificationmanager.h"
#include "Application/Managers/AppGrab/grabapplicationmanager.h"
#include "Common/ClientSelfUpdateManager/clientselfupdatemanager.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Common/Sorting/cleverapplicationiconpresentersortingdelegate.h"
#include "Application/Controllers/appterminationcontroller.h"
#include "Application/User/usermanager.h"
#include "Application/CriticalAction/criticalactionmanager.h"
#include "Common/guiconfigurationmanager.h"

GlobalObjectRegistry *GlobalObjectRegistry::sharedGlobalObjectRegistry()
{
    static GlobalObjectRegistry *globalObjectRegistry = NULL;

    if (!globalObjectRegistry) {
        globalObjectRegistry = new GlobalObjectRegistry();
    }
    return globalObjectRegistry;
}

GlobalObjectRegistry::GlobalObjectRegistry() :
    _pageNavigationManager(NULL),
    _menuManager(NULL),
    _notificationManager(NULL),
    _grabApplicationManager(NULL),
    _clientSelfUpdateManager(NULL),
    _localAppDatabaseManager(NULL),
    _cleverSortingDelegate(NULL),
    _appTerminationController(NULL),
    _userManager(NULL),
    _criticalActionManager(NULL)
{

}

GlobalObjectRegistry::~GlobalObjectRegistry()
{
    if(this->_pageNavigationManager != NULL) {
        this->_pageNavigationManager->setParent(NULL);
        delete this->_pageNavigationManager;
    }
    if(this->_menuManager != NULL) {
        this->_menuManager->setParent(NULL);
        delete this->_menuManager;
    }
    if(this->_notificationManager != NULL) {
        this->_notificationManager->setParent(NULL);
        delete this->_notificationManager;
    }
    if(this->_grabApplicationManager != NULL) {
        this->_grabApplicationManager->setParent(NULL);
        delete this->_grabApplicationManager;
    }
    if(this->_clientSelfUpdateManager != NULL) {
        this->_clientSelfUpdateManager->setParent(NULL);
        delete this->_clientSelfUpdateManager;
    }
    if(this->_localAppDatabaseManager != NULL) {
        this->_localAppDatabaseManager->setParent(NULL);
        delete this->_localAppDatabaseManager;
    }
    if(this->_cleverSortingDelegate != NULL) {
        this->_cleverSortingDelegate->setParent(NULL);
        delete this->_cleverSortingDelegate;
    }
    if(this->_appTerminationController != NULL) {
        this->_appTerminationController->setParent(NULL);
        delete this->_appTerminationController;
    }
}

PageNavigationManager *GlobalObjectRegistry::getPageNavigationManager() const
{
    return this->_pageNavigationManager;
}

void GlobalObjectRegistry::setPageNavigationManager(PageNavigationManager *pageNavigationManager)
{
    this->_pageNavigationManager = pageNavigationManager;
}

MenuManager *GlobalObjectRegistry::getMenuManager() const
{
    return this->_menuManager;
}

void GlobalObjectRegistry::setMenuManager(MenuManager *menuManager)
{
    this->_menuManager = menuManager;
}

NotificationManager *GlobalObjectRegistry::getNotificationManager() const {
    return this->_notificationManager;
}

void GlobalObjectRegistry::setNotificationManager(NotificationManager *notificationManager) {
    this->_notificationManager = notificationManager;
}

GrabApplicationManager *GlobalObjectRegistry::getGrabApplicationManager() const {
    return this->_grabApplicationManager;
}

void GlobalObjectRegistry::setGrabApplicationManager(GrabApplicationManager *grabApplicationManager) {
    this->_grabApplicationManager = grabApplicationManager;
}

ClientSelfUpdateManager *GlobalObjectRegistry::getClientSelfUpdateManager() {
    if(this->_clientSelfUpdateManager == NULL) {
        this->_clientSelfUpdateManager = new ClientSelfUpdateManager;
    }

    return this->_clientSelfUpdateManager;
}

LocalAppDatabaseManager *GlobalObjectRegistry::getLocalAppDatabaseManager() const {
    return this->_localAppDatabaseManager;
}

void GlobalObjectRegistry::setLocalAppDatabaseManager(LocalAppDatabaseManager *localAppDatabaseManager) {
    this->_localAppDatabaseManager = localAppDatabaseManager;
}

CleverApplicationIconPresenterSortingDelegate *GlobalObjectRegistry::getCleverSortingDelegate() const {
    return this->_cleverSortingDelegate;
}

void GlobalObjectRegistry::setCleverSortingDelegate(CleverApplicationIconPresenterSortingDelegate *cleverSortingDelegate) {
    this->_cleverSortingDelegate = cleverSortingDelegate;
}

AppTerminationController *GlobalObjectRegistry::getAppTerminationController() const {
    return this->_appTerminationController;
}

void GlobalObjectRegistry::setAppTerminationController(AppTerminationController *appTerminationController) {
    this->_appTerminationController = appTerminationController;
}

UserManager *GlobalObjectRegistry::getUserManager() const {
    return this->_userManager;
}

void GlobalObjectRegistry::setUserManager(UserManager *userManager) {
    this->_userManager = userManager;
}

CriticalActionManager *GlobalObjectRegistry::getCriticalActionManager() const {
    return this->_criticalActionManager;
}

void GlobalObjectRegistry::setCriticalActionManager(CriticalActionManager *criticalActionManager) {
    this->_criticalActionManager = criticalActionManager;
}

GuiConfigurationManager *GlobalObjectRegistry::getGuiConfigurationManager() const {
    return this->_guiConfigurationManager;
}
void GlobalObjectRegistry::setGuiConfigurationManager(GuiConfigurationManager *guiConfigurationManager) {
    this->_guiConfigurationManager = guiConfigurationManager;
}
