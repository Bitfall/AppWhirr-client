#include "frameworkinstaller.h"

#include <QThread>
#include <QMetaClassInfo>

#include "backgroundframeworkinstaller.h"

FrameworkInstaller::FrameworkInstaller(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType< FrameworkCheckModel >("FrameworkCheckModel");
    qRegisterMetaType< FrameworkInfo >("FrameworkInfo");
}

void FrameworkInstaller::startFrameworkInstallAsync(FrameworkCheckModel frameworkInstallModel)
{
    QThread *fwInstallerBgThread = new QThread;
    BackgroundFrameworkInstaller *fwInstaller = new BackgroundFrameworkInstaller(frameworkInstallModel);

    // connect
    connect(fwInstallerBgThread, SIGNAL(started()), fwInstaller, SLOT(startFrameworkInstall()));

    connect(fwInstaller, SIGNAL(finishedWithSuccess(FrameworkCheckModel)), this, SIGNAL(frameworkInstalled(FrameworkCheckModel)));
    connect(fwInstaller, SIGNAL(finishedWitError(FrameworkCheckModel,QString)), this, SIGNAL(frameworkInstallFailed(FrameworkCheckModel,QString)));

    // destruct
    connect(fwInstaller, SIGNAL(finished()), fwInstallerBgThread, SLOT(quit()));
    connect(fwInstaller, SIGNAL(finished()), this, SLOT(_installFinished()));

    fwInstaller->moveToThread(fwInstallerBgThread);
    fwInstallerBgThread->start();
}

void FrameworkInstaller::_installFinished()
{
    if(this->sender() != NULL) {
        delete this->sender();
    }
}
