#ifndef GLOBALOBJECTREGISTRY_H
#define GLOBALOBJECTREGISTRY_H

class PageNavigationManager;
class MenuManager;
class NotificationManager;
class GrabApplicationManager;
class ClientSelfUpdateManager;
class LocalAppDatabaseManager;
class CleverApplicationIconPresenterSortingDelegate;
class AppTerminationController;
class UserManager;
class CriticalActionManager;
class GuiConfigurationManager;

//
// !!! NOT THEAD SAFE !!!
//  -> usage: set the items from 1 place and only once, then you can get everywhere you want
class GlobalObjectRegistry
{
private:
    explicit GlobalObjectRegistry();
    ~GlobalObjectRegistry();

public:
    static GlobalObjectRegistry* sharedGlobalObjectRegistry();

    //
    PageNavigationManager *getPageNavigationManager() const;
    void setPageNavigationManager(PageNavigationManager *pageNavigationManager);
    //
    MenuManager *getMenuManager() const;
    void setMenuManager(MenuManager *menuManager);
    //
    NotificationManager *getNotificationManager() const;
    void setNotificationManager(NotificationManager *notificationManager);
    //
    GrabApplicationManager *getGrabApplicationManager() const;
    void setGrabApplicationManager(GrabApplicationManager *grabApplicationManager);
    //
    ClientSelfUpdateManager *getClientSelfUpdateManager();
    //
    LocalAppDatabaseManager *getLocalAppDatabaseManager() const;
    void setLocalAppDatabaseManager(LocalAppDatabaseManager *localAppDatabaseManager);
    //
    CleverApplicationIconPresenterSortingDelegate *getCleverSortingDelegate() const;
    void setCleverSortingDelegate(CleverApplicationIconPresenterSortingDelegate *cleverSortingDelegate);
    //
    AppTerminationController *getAppTerminationController() const;
    void setAppTerminationController(AppTerminationController *appTerminationController);
    //
    UserManager *getUserManager() const;
    void setUserManager(UserManager *userManager);
    //
    CriticalActionManager *getCriticalActionManager() const;
    void setCriticalActionManager(CriticalActionManager *criticalActionManager);
    //
    GuiConfigurationManager *getGuiConfigurationManager() const;
    void setGuiConfigurationManager(GuiConfigurationManager *guiConfigurationManager);

private:
    PageNavigationManager *_pageNavigationManager;
    MenuManager *_menuManager;
    NotificationManager *_notificationManager;
    GrabApplicationManager *_grabApplicationManager;
    ClientSelfUpdateManager *_clientSelfUpdateManager;
    LocalAppDatabaseManager *_localAppDatabaseManager;
    CleverApplicationIconPresenterSortingDelegate *_cleverSortingDelegate;
    AppTerminationController *_appTerminationController;
    UserManager *_userManager;
    CriticalActionManager *_criticalActionManager;
    GuiConfigurationManager *_guiConfigurationManager;
};

#endif // GLOBALOBJECTREGISTRY_H
