#ifndef INSTALLATION_H
#define INSTALLATION_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
class QTimer;
QT_END_NAMESPACE

class LocalMessageSender;
class ApplicationInstaller;

#include "../EasyAppsClient/Application/Managers/AppGrab/appinstallstates.h"

class AppInstallManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(AppInstallStates::AppInstallStatesEnum)

public:
    explicit AppInstallManager(QObject *parent);

    void startInstallZip(const QString& zipPath, QString appId);

Q_SIGNALS:
    void finished();
    void failed();

public Q_SLOTS:
    void installationFailed(QString errorMessage, ApplicationInstaller* sender);
    void installationSucceeded(ApplicationInstaller* sender);
    void hasARunningCopy();
private Q_SLOTS:
    void _installProgressState(AppInstallStates::AppInstallStatesEnum installState);

private Q_SLOTS:
    void localInterProcessCommunicationFinished();
    void localInterProcessCommunicationFailedWithError(QString message);

    void _sendStillProcessingMessage();

private:
    LocalMessageSender*          _localMessageSender;
    ApplicationInstaller*   applicationInstaller;
    QString                 zipPath;
    QString                 appId;
    QTimer                  *_stillProcessingTimer;
    bool                    _stillProcessingMessageWasSent;
    int _remainingMaxIpcErrorsBeforeAbort;
};

#endif // INSTALLATION_H
