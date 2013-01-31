#include "uninstallingviewcontroller.h"

#include <QMetaType>
#include <QThreadPool>

#include "../uninstallconfigs.h"
#include "../View/uninstallingview.h"
#include "../EasyAppsClient/Common/Runnables/runnable_checkforrunningexecutable.h"
#include "../awuninstallperformertask.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

UninstallingViewController::UninstallingViewController(UninstallingView *view, UninstallConfigs *uninstallConfigs, QObject *parent) :
    ViewControllerInterface(parent),
    _view(view),
    _uninstallConfigs(uninstallConfigs)
{
    connect(_view, SIGNAL(forceUninstallButtonClicked()), this, SLOT(forceUninstallButtonClicked()));
    connect(_view, SIGNAL(recheckRunningAppsButtonClicked()), this, SLOT(recheckRunningAppsButtonClicked()));
}

QWidget *UninstallingViewController::getView() const
{
    return _view;
}

void UninstallingViewController::destroyView()
{
    AW_QOBJECT_SAFE_DELETE( _view );
}

void UninstallingViewController::startUninstall()
{
    this->checkForUninstallThreats();
}

void UninstallingViewController::checkForUninstallThreats()
{
    if( _uninstallConfigs->getIsRemoveInstalledAppsAsWell() ) {
        this->checkForRunningApps();
    }
    else {
        // no threat check required, currently only running-app detection is part of the threat-check
        this->noMoreUninstallThreatsStartTheUninstall();
    }
}

void UninstallingViewController::noMoreUninstallThreatsStartTheUninstall()
{
    _view->setStatusText(tr("No running apps found.\n\nUninstall started..."));
    AWUninstallPerformerTask *uninstallTask = new AWUninstallPerformerTask(_uninstallConfigs);
    connect(uninstallTask, SIGNAL(uninstallStateMessage(QString)), this, SLOT(uninstallStateMessageRecieved(QString)));
    connect(uninstallTask, SIGNAL(finished()), this, SLOT(finishedWithUninstallTask()));
    QThreadPool::globalInstance()->start(uninstallTask);
}

//void UninstallingViewController::removeAppWhirrClientAndLocalFiles()
//{
//    _view->setStatusText(tr("Deleting the AppWhirr app..."));
//}

void UninstallingViewController::checkForRunningApps()
{
    _view->setStatusText(tr("Scanning for running apps..."));
    qRegisterMetaType< QList<PlatformspecificRunningExecutableDataInterface*> >("QList<PlatformspecificRunningExecutableDataInterface*>");
    Runnable_CheckForRunningExecutable *checkForRunningAppsWorker = new Runnable_CheckForRunningExecutable(AppSpecificPathHelper::getLocalRootDirectory());
    connect(checkForRunningAppsWorker, SIGNAL(finishedWithSuccess(QList<PlatformspecificRunningExecutableDataInterface*>)), this, SLOT(checkForRunningAppsWorkerFinishedWithResult(QList<PlatformspecificRunningExecutableDataInterface*>)));
    QThreadPool::globalInstance()->start(checkForRunningAppsWorker);
}

void UninstallingViewController::checkForRunningAppsWorkerFinishedWithResult(QList<PlatformspecificRunningExecutableDataInterface *> resultPlatformSpecificRunningAppInfos)
{
    if(resultPlatformSpecificRunningAppInfos.isEmpty()) {
        _view->setStatusText(tr("No running apps found - uninstall started..."));
        this->noMoreUninstallThreatsStartTheUninstall();
    }
    else {
        _view->setStatusText(tr("Running apps found. Quit them and click the 'Check for runing apps again' button to check whether every app is closed or you can click the 'Uninstall anyway' button and let the uninstaller remove as many apps and files as it can."));
        _view->showActionBar();
        // present warning + button: remove as much as possible
    }
}

void UninstallingViewController::uninstallStateMessageRecieved(QString stateMessage)
{
    _view->setStatusText(stateMessage);
}

void UninstallingViewController::finishedWithUninstallTask()
{
    Q_EMIT finishedWithUninstall();
}

void UninstallingViewController::forceUninstallButtonClicked()
{
    _view->hideActionBar();
    _view->setStatusText(tr("Uninstalling..."));
    this->noMoreUninstallThreatsStartTheUninstall();
}

void UninstallingViewController::recheckRunningAppsButtonClicked()
{
    _view->hideActionBar();
    _view->setStatusText(tr("Uninstall restarted..."));
    this->startUninstall();
}

//void UninstallingViewController::finishedWithRemoveInstalledApps()
//{
//    // start the AppWhirr client uninstall
//    if( _uninstallConfigs->getIsRemoveAppWhirrAndLocalData() ){
//        this->removeAppWhirrClientAndLocalFiles();
//    }
//    else {
//        // or: we're done
//        Q_EMIT finishedWithUninstall();
//    }
//}
