#ifndef UNINSTALLINGVIEWCONTROLLER_H
#define UNINSTALLINGVIEWCONTROLLER_H

#include "viewcontrollerinterface.h"

class PlatformspecificRunningExecutableDataInterface;

class UninstallingView;
class UninstallConfigs;
class UninstallingViewController : public ViewControllerInterface
{
    Q_OBJECT
public:
    explicit UninstallingViewController(UninstallingView *view, UninstallConfigs *uninstallConfigs, QObject *parent = 0);

    virtual QWidget *getView() const;
    virtual void destroyView();

    void startUninstall();

Q_SIGNALS:
    void finishedWithUninstall();

private Q_SLOTS:
    void checkForUninstallThreats();
    void noMoreUninstallThreatsStartTheUninstall();
//    void removeAppWhirrClientAndLocalFiles();
//    void removeAppWhirrApps();
    void checkForRunningApps();
    void checkForRunningAppsWorkerFinishedWithResult(QList<PlatformspecificRunningExecutableDataInterface *> resultPlatformSpecificRunningAppInfos);

    void uninstallStateMessageRecieved(QString stateMessage);
    void finishedWithUninstallTask();

    void forceUninstallButtonClicked();
    void recheckRunningAppsButtonClicked();


//    void removeInstalledApps();
//    void finishedWithRemoveInstalledApps();

//    void removeAppWhirrClientApp();
//    void removeAppWhirrLocalDataExceptUsersData();

private:
    UninstallingView *_view;
    UninstallConfigs *_uninstallConfigs;
};

#endif // UNINSTALLINGVIEWCONTROLLER_H
