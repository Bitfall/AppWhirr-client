#ifndef RUNNABLE_APPINSTALLCLEANUPAFTERERROR_H
#define RUNNABLE_APPINSTALLCLEANUPAFTERERROR_H

#include <QObject>
#include <QRunnable>

#include "../installfallbackstatesenum.h"

class Runnable_AppInstallCleanupAfterError : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Runnable_AppInstallCleanupAfterError(InstallFallbackStates::InstallFallbackStatesEnum fallbackState, QString appId, QString givenErrorMessage, QObject *parent = 0);

    void run();

Q_SIGNALS:
    void finished(QString givenErrorMessage);

private:
    InstallFallbackStates::InstallFallbackStatesEnum _fallbackState;
    QString _appId;
    QString _givenErrorMessage;

};

#endif // RUNNABLE_APPINSTALLCLEANUPAFTERERROR_H
