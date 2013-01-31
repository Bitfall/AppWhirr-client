#include <QDebug>
#include <QCoreApplication>
#include <QSettings>

#include "settings.h"
#include "Application/appspecificpathhelper.h"

#include "Common/guiconfigurationmanager.h"
#include <QApplication>
#include <QDesktopWidget>

//
// defines
#define AUTO_START_APPLICATION          QString("application/autoStartApplication")
#define LAST_USED_CLIENT_VERSION          QString("application/lastUsedClientVersion")

#define SHOW_LAUNCHER_ON_STARTUP        QString("launcher/showLauncherOnStartup")
#define ADD_SYSTEM_TRAY_TO_LAUNCHER     QString("launcher/addSystemTrayToLauncher")
//#define SETUP_HOTKEY_FOR_LAUNCHER       QString("launcher/setupHotkeyForLauncher")
#define IS_HIDE_LAUNCHER_RATHER_THAN_MINIMIZE QString("launcher/isHideRatherThanMinimize")
#define IS_SET_GLOBAL_HOTKEY_TO_LAUNCHER QString("launcher/isSetGlobalHotkeyToLauncher")
#define IS_SHOW_LAUNCHER_HIDE_SYSTEM_TRAY_MESSAGE_ONLY_ONCE QString("launcher/isShowLauncherHideSystemTrayMessageOnlyOnce")
#define MOUSE_SCROLL_SPEED_ADJUSTMENT_VALUE QString("launcher/mouseScrollSpeedAdjustmentValue")
#define IS_REMEMBER_LAST_USER QString("user/isRememberLastUser")

#define BROWSER_WINDOW_GEOMETRY          QString("browser/geometry")

Settings* Settings::sharedSettings() {
    static Settings* settings = new Settings();

    return settings;
}

Settings::Settings()
    : SettingsBase()
{
    _initSettings();
    _setupDefaults();
}

void Settings::_initSettings() {
    DLog("settings started");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       AppSpecificPathHelper::getSettingsDirectory());

    QCoreApplication::setOrganizationName("Bitfall");
    QCoreApplication::setOrganizationDomain("bitfall.com");
    QCoreApplication::setApplicationName("AppWhirr");

    qSettings = new QSettings();
}

void Settings::_setupDefaults() {
    //
    // Application

    if (!qSettings->contains(AUTO_START_APPLICATION)) {
        //setSettingsValue(AUTO_START_APPLICATION, 1);
        setIsAutoStartAtSystemStartup(true);
    }

    if (!qSettings->contains(LAST_USED_CLIENT_VERSION)) {
        this->setLastUsedClientVersion("0.0");
    }

    //
    // Launcher

    if (!qSettings->contains(SHOW_LAUNCHER_ON_STARTUP)) {
#ifdef Q_WS_WIN
        //setSettingsValue(SHOW_LAUNCHER_ON_STARTUP, 1);
        setIsShowLauncherAtStartup(true);
#endif

#ifdef Q_WS_MAX
        //setSettingsValue(SHOW_LAUNCHER_ON_STARTUP, 0);
        setIsShowLauncherAtStartup(false);
#endif
    }

    if (!qSettings->contains(ADD_SYSTEM_TRAY_TO_LAUNCHER)) {
        //setSettingsValue(ADD_SYSTEM_TRAY_TO_LAUNCHER, 1);
        setIsAddSystemTrayToLauncher(true);
    }

    if (!qSettings->contains(IS_SET_GLOBAL_HOTKEY_TO_LAUNCHER)) {
        //setSettingsValue(SETUP_HOTKEY_FOR_LAUNCHER, 1);
        setIsSetGlobalHotkeyToLauncher(true);
    }

    if(!qSettings->contains(IS_HIDE_LAUNCHER_RATHER_THAN_MINIMIZE)) {
#ifdef Q_WS_WIN
        setIsHideLauncherRatherThanMinimize(false);
#endif

#ifdef Q_WS_MAC
        setIsHideLauncherRatherThanMinimize(true);
#endif
    }

    if(!qSettings->contains(IS_SHOW_LAUNCHER_HIDE_SYSTEM_TRAY_MESSAGE_ONLY_ONCE))
    {
        setIsShowLauncherHideSystemTrayMessageOnlyOnce(false);
    }

    if(!qSettings->contains(MOUSE_SCROLL_SPEED_ADJUSTMENT_VALUE))
    {
        setMouseScrollSpeedAdjustmentValue(3);
    }

    if(!qSettings->contains(IS_REMEMBER_LAST_USER))
    {
        setIsRememberLastUser(true);
    }

    //
    // Browser
    if (!qSettings->contains(BROWSER_WINDOW_GEOMETRY)) {
        GuiConfigurationManager *guiConfManager = new GuiConfigurationManager(":/GuiConfigurations");

        // set default size
        QSize size = guiConfManager->getSizeForId("MainWindow/defaultSize");
        QRect availableRect = QApplication::desktop()->availableGeometry();

        setBrowserWindowGeometry(QRect((int)(availableRect.width() - size.width()) / 2,
                                       (int)(availableRect.height() - size.height()) / 2,
                                       size.width(),
                                       size.height()));

        delete guiConfManager;
    }
}

// -------------------------------
// settings get/set methods

bool Settings::getIsAutoStartAtSystemStartup() const
{
    return this->getBooleanValue(AUTO_START_APPLICATION);
}

void Settings::setIsAutoStartAtSystemStartup(bool isAutoStartAtSystemStartup) const
{
    this->setSettingsValue(AUTO_START_APPLICATION, isAutoStartAtSystemStartup);
}

bool Settings::getIsHideLauncherRatherThanMinimize() const
{
    return this->getBooleanValue(IS_HIDE_LAUNCHER_RATHER_THAN_MINIMIZE);
}

void Settings::setIsHideLauncherRatherThanMinimize(bool isHideRatherThanMinimize) const
{
    this->setSettingsValue(IS_HIDE_LAUNCHER_RATHER_THAN_MINIMIZE, isHideRatherThanMinimize);
}

bool Settings::getIsSetGlobalHotkeyToLauncher() const
{
    return this->getBooleanValue(IS_SET_GLOBAL_HOTKEY_TO_LAUNCHER);
}

void Settings::setIsSetGlobalHotkeyToLauncher(bool newState) const
{
    this->setSettingsValue(IS_SET_GLOBAL_HOTKEY_TO_LAUNCHER, newState);
}

bool Settings::getIsShowLauncherAtStartup() const
{
    return this->getBooleanValue(SHOW_LAUNCHER_ON_STARTUP);
}

void Settings::setIsShowLauncherAtStartup(bool newState) const
{
    this->setSettingsValue(SHOW_LAUNCHER_ON_STARTUP, newState);
}

bool Settings::getIsAddSystemTrayToLauncher() const
{
    return this->getBooleanValue(ADD_SYSTEM_TRAY_TO_LAUNCHER);
}

void Settings::setIsAddSystemTrayToLauncher(bool newState) const
{
    this->setSettingsValue(ADD_SYSTEM_TRAY_TO_LAUNCHER, newState);
}

bool Settings::getIsShowLauncherHideSystemTrayMessageOnlyOnce() const
{
    return this->getBooleanValue(IS_SHOW_LAUNCHER_HIDE_SYSTEM_TRAY_MESSAGE_ONLY_ONCE);
}

void Settings::setIsShowLauncherHideSystemTrayMessageOnlyOnce(bool newState)
{
    this->setSettingsValue(IS_SHOW_LAUNCHER_HIDE_SYSTEM_TRAY_MESSAGE_ONLY_ONCE, newState);
}

int Settings::getMouseScrollSpeedAdjustmentValue() const
{
    return this->getIntegerValue(MOUSE_SCROLL_SPEED_ADJUSTMENT_VALUE);
}

void Settings::setMouseScrollSpeedAdjustmentValue(int value) const
{
    this->setSettingsValue(MOUSE_SCROLL_SPEED_ADJUSTMENT_VALUE, value);
}

bool Settings::getIsRememberLastUser() const {
    return this->getBooleanValue(IS_REMEMBER_LAST_USER);
}

void Settings::setIsRememberLastUser(bool isRememberLastUser) const {
    this->setSettingsValue(IS_REMEMBER_LAST_USER, isRememberLastUser);
}

QRect Settings::getBrowserWindowGeometry() const {
    return this->getRectValue(BROWSER_WINDOW_GEOMETRY);
}

void Settings::setBrowserWindowGeometry(QRect browserGeometry) const {
    this->setSettingsValue(BROWSER_WINDOW_GEOMETRY, browserGeometry);
}

QString Settings::getLastUsedClientVersion() const
{
    return this->getQStringValue(LAST_USED_CLIENT_VERSION);
}

void Settings::setLastUsedClientVersion(QString value)
{
    this->setSettingsValue(LAST_USED_CLIENT_VERSION, value);
}
