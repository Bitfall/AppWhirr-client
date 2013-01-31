#ifndef BACKGROUND_INSTALLER_H
#define BACKGROUND_INSTALLER_H

#include <QObject>
#include <QRunnable>

#include "../EasyAppsClient/Application/Managers/AppGrab/appinstallstates.h"

class Background_AppInstaller : public QObject, public QRunnable {
    Q_OBJECT
    Q_ENUMS(AppInstallStates::AppInstallStatesEnum)

public:
    explicit Background_AppInstaller(QString appId, QString zipFileName, QObject *parent=0);

    void run();

Q_SIGNALS:
    void failedWithError(QString errorMessage);
    // will emit signals when completes steps
    void completedBackupStep();
    void finishedSuccessfully();
    void installProgressState(AppInstallStates::AppInstallStatesEnum progressState);

protected:
    QString _appId;
    QString _zipFileName;
};

#endif // BACKGROUND_INSTALLER_H
