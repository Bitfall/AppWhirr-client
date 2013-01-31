#include "uninstallconfigs.h"

UninstallConfigs::UninstallConfigs(QObject *parent) :
    QObject(parent),
    _isRemoveInstalledAppsAsWell(false),
    _isRemoveAppWhirrAndLocalData(true)
{
}

void UninstallConfigs::setIsRemoveInstalledAppsAsWell(bool value)
{
    _isRemoveInstalledAppsAsWell = value;
}

bool UninstallConfigs::getIsRemoveInstalledAppsAsWell() const
{
    return _isRemoveInstalledAppsAsWell;
}

void UninstallConfigs::setIsRemoveAppWhirrAndLocalData(bool value)
{
    _isRemoveAppWhirrAndLocalData = value;
}

bool UninstallConfigs::getIsRemoveAppWhirrAndLocalData() const
{
    return _isRemoveAppWhirrAndLocalData;
}

void UninstallConfigs::setAppWhirrClientAppFolderPath(QString value)
{
    _appWhirrClientAppFolderPath = value;
}

QString UninstallConfigs::getAppWhirrClientAppFolderPath()
{
    return _appWhirrClientAppFolderPath;
}

void UninstallConfigs::addFailedUninstallStep(UninstallDeleteSteps::UninstallDeleteStepsEnum step)
{
    _failedUninstallSteps.insert(step);
}

QSet<UninstallDeleteSteps::UninstallDeleteStepsEnum> UninstallConfigs::getFailedUninstallSteps()
{
    return _failedUninstallSteps;
}
