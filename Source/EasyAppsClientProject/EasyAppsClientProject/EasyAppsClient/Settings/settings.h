#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QRect>
#include <QSize>
#include <QVariant>

#include "Common/Settings/settingsbase.h"

class QSettings;

class Settings : public SettingsBase
{
public:
    static Settings* sharedSettings();

    // -------------------------------
    // settings get/set methods

    // const because this class doesn't hold any settings, only passes it
    // to QSettings

    bool getIsAutoStartAtSystemStartup() const;
    void setIsAutoStartAtSystemStartup(bool isAutoStartAtSystemStartup) const;

    bool getIsHideLauncherRatherThanMinimize() const;
    void setIsHideLauncherRatherThanMinimize(bool isHideRatherThanMinimize) const;

    bool getIsSetGlobalHotkeyToLauncher() const;
    void setIsSetGlobalHotkeyToLauncher(bool newState) const;

    bool getIsShowLauncherAtStartup() const;
    void setIsShowLauncherAtStartup(bool newState) const;

    bool getIsAddSystemTrayToLauncher() const;
    void setIsAddSystemTrayToLauncher(bool newState) const;

    bool getIsShowLauncherHideSystemTrayMessageOnlyOnce() const;
    void setIsShowLauncherHideSystemTrayMessageOnlyOnce(bool newState);

    int getMouseScrollSpeedAdjustmentValue() const;
    void setMouseScrollSpeedAdjustmentValue(int value) const;

    bool getIsRememberLastUser() const;
    void setIsRememberLastUser(bool isRememberLastUser) const;

    QRect getBrowserWindowGeometry() const;
    void setBrowserWindowGeometry(QRect browserGeometry) const;

    /*! Get the previously used client version.

        @note Update this info when the client is started and no new version found / no client update will be performed. This way we can track whether this is the first run of this version.

        @note This can be also '0.0', which means the client runs for the first time on this system.
    */
    QString getLastUsedClientVersion() const;

    /*! Set the previously used client version.

        @note Update this info when the client is started and no new version found / no client update will be performed. This way we can track whether this is the first run of this version.

        @note This can be also '0.0', which means the client runs for the first time on this system.
    */
    void setLastUsedClientVersion(QString value);

protected:
    void _initSettings();
    void _setupDefaults();

private:
    Settings();
};

#endif // SETTINGS_H
