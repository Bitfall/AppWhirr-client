#include "configmanager.h"

ConfigManager::ConfigManager()
{
}

// --------------------
// --- client infos ---
QString ConfigManager::getClientVersion() {
    return "0.8.7.2";
}

//
QString ConfigManager::getClientTrayIconTooltipText() {
    return QObject::tr("AppWhirr - Rule your apps");
}


// ------------------------------------------
// --- periodical communication intervals ---

quint32 ConfigManager::getStoreMainPageContentReloadIntervalMilliseconds() {
#ifdef USE_NETWORK_REFRESH_TIMES_FOR_TESTING
    return 1000 * 10; // test - 30 secs
#else
    return 1000 * 60 * 60; // every hour
#endif
}

quint32 ConfigManager::getClientSelfUpdateCheckIntervalMilliseconds() {
#ifdef USE_NETWORK_REFRESH_TIMES_FOR_TESTING
    return 1000 * 10; // internal test interval (10 seconds)
#else
    return 1000 * 60 * 60; // every hour
#endif
}


// ------------
// --- menu ---

bool ConfigManager::getMenuItemIsActivateIfThatsTheCurrentOneWhenRequested() {
    // IMPORTANT: if it's set to 'true': can cause infinite recursion if the menu related menu-item-activated
    //  signal handler performs some action which results to re-activate the same menu item
    return false;
}

// --------------
// --- search ---

quint32 ConfigManager::getSearchMinimumFilterTextLengthToPerformSearch() {
    return 3;
}

// --------------------------
// --- general networking ---

quint32 ConfigManager::getNetworkDefaultTimeoutMilliseconds() {
    return 1000 * 15; // 15 seconds
}


// ---------------------------
// --- notification system ---

quint32 ConfigManager::getNotificationWidgetDefaultTimeoutMilliseconds() {
    return 1000 * 10; // 10 seconds
}

quint32 ConfigManager::getNotificationManagerUpdateIntervalMilliseconds() {
    return 1000 * 2; // 2 seconds
}

quint32 ConfigManager::getNotificationManagerOrderUpdateCycles() {
    return 3;
}

int ConfigManager::getNotificationManagerMaximumVisibleNotificationItemCount() {
    return 3;
}

// --------------------------------
// --- app install and handling ---

//
// the name of the special static app-info file contains infos about the related app
QString ConfigManager::getStaticAppInfoFileNameInInstallPack() {
    return "appInfo.sqlite";
}



// ------------------------
// --- windows specific ---

#ifdef Q_WS_WIN

int ConfigManager::_win_getAppwhirrClientWindowsGlobalShortcutId() {
    return 128;
}

#endif
