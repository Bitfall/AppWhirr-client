#include "configmanager.h"

ConfigManager::ConfigManager()
{
}

// --------------------
// --- client infos ---
QString ConfigManager::getClientVersion() {
    return "0.9.17";
}

QString ConfigManager::getServerApiVersion()
{
    return "1.1";
}

//
QString ConfigManager::getClientTrayIconTooltipText() {
    return QObject::tr("AppWhirr - Rule your apps");
}


// ------------------------------------------
// --- periodical communication intervals ---

quint32 ConfigManager::getStoreMainPageContentReloadIntervalMilliseconds() {
#ifdef USE_NETWORK_REFRESH_TIMES_FOR_TESTING
    return 1000 * 10; // test - 10 secs
#else
    return 1000 * 60 * 60; // every hour
#endif
}

quint32 ConfigManager::getClientSelfUpdateCheckIntervalMilliseconds() {
#ifdef USE_NETWORK_REFRESH_TIMES_FOR_TESTING
    return 1000 * 60; // internal test interval (60 seconds)
#else
    return 1000 * 60 * 60; // every hour
#endif
}

quint32 ConfigManager::getAppDatabaseSyncIntervalMilliseconds()
{
#ifdef USE_NETWORK_REFRESH_TIMES_FOR_TESTING
    return 1000 * 60; // internal test interval (60 seconds)
#else
    return 1000 * 60 * 60; // every hour
#endif
}

quint32 ConfigManager::getUserAppDataSyncIntervalMilliseconds()
{
#ifdef USE_NETWORK_REFRESH_TIMES_FOR_TESTING
    return 1000 * 60 * 10; // internal test interval (10 minutes)
#else
    return 1000 * 60 * 10; // 10 minutes
#endif
}

quint32 ConfigManager::getUserGrabAppAuthorizationTimeoutIntervalMilliseconds()
{
//    return 1000 * 60;
    return 1000 * 60 * 5; // 10 minutes
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
//    return 3;
    return 1;
}

// --------------------------
// --- general networking ---

quint32 ConfigManager::getNetworkDefaultTimeoutMilliseconds() {
    return 1000 * 15; // 15 seconds
}

quint32 ConfigManager::getNetworkDefaultMaximumParallelRequests()
{
    return 3;
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

QString ConfigManager::getFrameworkCheckerLauncherExecutableFileNameWithoutExtension()
{
    return "aw_fwcheck_runner";
}

bool ConfigManager::getIsEnableFlashInContents()
{
#ifdef Q_OS_WIN
    return true;
//    return false;
#endif

#ifdef Q_OS_MAC
    return false;
#endif
}

int ConfigManager::getRestartAppWhirrClient_returnCode()
{
    return 1654;
}



// ------------------------
// --- windows specific ---

#ifdef Q_WS_WIN

int ConfigManager::_win_getAppwhirrClientWindowsGlobalShortcutId() {
    return 128;
}

#endif
