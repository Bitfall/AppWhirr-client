#ifndef UNINSTALLCONFIGS_H
#define UNINSTALLCONFIGS_H

#include <QObject>
#include <QSet>

#include "uninstalldeletesteps.h"

class UninstallConfigs : public QObject
{
    Q_OBJECT
public:
    explicit UninstallConfigs(QObject *parent = 0);

    void setIsRemoveInstalledAppsAsWell(bool value);
    bool getIsRemoveInstalledAppsAsWell() const;

    void setIsRemoveAppWhirrAndLocalData(bool value);
    bool getIsRemoveAppWhirrAndLocalData() const;

    void setAppWhirrClientAppFolderPath(QString value);
    QString getAppWhirrClientAppFolderPath();

    void addFailedUninstallStep(UninstallDeleteSteps::UninstallDeleteStepsEnum step);
    QSet<UninstallDeleteSteps::UninstallDeleteStepsEnum> getFailedUninstallSteps();


private:
    bool _isRemoveInstalledAppsAsWell;
    bool _isRemoveAppWhirrAndLocalData;
    QString _appWhirrClientAppFolderPath;
    QSet<UninstallDeleteSteps::UninstallDeleteStepsEnum> _failedUninstallSteps;
};

#endif // UNINSTALLCONFIGS_H
