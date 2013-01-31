#ifndef BACKGROUNDCLIENTINSTALLER_H
#define BACKGROUNDCLIENTINSTALLER_H

#include <QObject>
#include <QRunnable>

#include "installmajorstepsenum.h"

class Background_ClientInstaller : public QObject, public QRunnable
{
    Q_OBJECT

public:
    //
    // clientRootPath is the current root path of the client, where the old version is, and where the new version will be installed
    explicit Background_ClientInstaller(QString clientRootPath, InstallMajorSteps continueFromThisStep = DoFromTheBeginning, QObject *parent = 0);

    void run();

Q_SIGNALS:
    void succeeded();
    //
    // afterThisStepWasDone is a "checkpoint" what you can support to "retry" install only from this checkpoint
    void failedWithErrorMessage(QString errorMessage, InstallMajorSteps lastStepDone);

    void installPercentage(quint32 percentage);

private:
    bool tryToBackup();
    bool tryToRemoveOldVersion();
    bool copyNewVersion();
    bool setRequiredPermissionsForTheNewVersion();

private:
    QString _clientRootPath;
    InstallMajorSteps _lastStepDone;
};

#endif // BACKGROUNDCLIENTINSTALLER_H
