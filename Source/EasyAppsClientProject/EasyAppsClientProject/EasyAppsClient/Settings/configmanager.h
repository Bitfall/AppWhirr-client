#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>

//
// contains project specific configurations which cannot be changed during runtime
//  note: for changeable settings (which can be changed during runtime) use the Settings class
class ConfigManager
{
private:
    ConfigManager();

public:

    // --------------------
    // --- client infos ---
    static QString getClientVersion();
    static QString getServerApiVersion();

    //
    static QString getClientTrayIconTooltipText();

    // ------------------------------------------
    // --- periodical communication intervals ---
    static quint32 getStoreMainPageContentReloadIntervalMilliseconds();
    static quint32 getClientSelfUpdateCheckIntervalMilliseconds();
    static quint32 getAppDatabaseSyncIntervalMilliseconds();
    static quint32 getUserAppDataSyncIntervalMilliseconds();

    static quint32 getUserGrabAppAuthorizationTimeoutIntervalMilliseconds();

    // ------------
    // --- menu ---

    //
    // IMPORTANT: if it's set to 'true': can cause infinite recursion if the menu related menu-item-activated
    //  signal handler performs some action which results to re-activate the same menu item
    static bool getMenuItemIsActivateIfThatsTheCurrentOneWhenRequested();

    // --------------
    // --- search ---

    //
    // if the search filter text length is less then DON'T start search-by-filter
    static quint32 getSearchMinimumFilterTextLengthToPerformSearch();

    // --------------------------
    // --- general networking ---
    static quint32 getNetworkDefaultTimeoutMilliseconds();
    static quint32 getNetworkDefaultMaximumParallelRequests();

    // ---------------------------
    // --- notification system ---
    static quint32 getNotificationWidgetDefaultTimeoutMilliseconds();
    static quint32 getNotificationManagerUpdateIntervalMilliseconds();
    //
    // defines how many update cycles have to be done to update the order of items
    static quint32 getNotificationManagerOrderUpdateCycles();
    static int getNotificationManagerMaximumVisibleNotificationItemCount();

    // --------------------------------
    // --- app install and handling ---

    //
    // the name of the special static app-info file contains infos about the related app
    static QString getStaticAppInfoFileNameInInstallPack();

    static QString getFrameworkCheckerLauncherExecutableFileNameWithoutExtension();

    // ------------------------------
    // --- content configurations ---
    static bool getIsEnableFlashInContents();

    // ---------------------
    // --- app internals ---
    static int getRestartAppWhirrClient_returnCode();


    // ------------------------
    // --- windows specific ---
#ifdef Q_WS_WIN
    //
    // for global keyboard hotkey/shortcut-key registration
    static int _win_getAppwhirrClientWindowsGlobalShortcutId();
#endif
};

#endif // CONFIGMANAGER_H
