#include "availableframeworkchecker.h"

#include <QThread>
#include <QMetaClassInfo>

#include "backgroundframeworkcheck.h"

AvailableFrameworkChecker::AvailableFrameworkChecker(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType< QList<FrameworkCheckModel> >("QList<FrameworkCheckModel>");
    qRegisterMetaType< FrameworkCheckModel >("FrameworkCheckModel");
    qRegisterMetaType< FrameworkInfo >("FrameworkInfo");
}

void AvailableFrameworkChecker::checkForFrameworksAsync(QList<FrameworkCheckModel> frameworkCheckModels)
{
    QThread *checkerBgThread = new QThread;
    BackgroundFrameworkCheck *fwChecker = new BackgroundFrameworkCheck(frameworkCheckModels);

    // connect
    connect(checkerBgThread, SIGNAL(started()), fwChecker, SLOT(startChecking()));

    connect(fwChecker, SIGNAL(frameworksAvailable(QList<FrameworkCheckModel>)), this, SIGNAL(frameworksAvailable(QList<FrameworkCheckModel>)));
    connect(fwChecker, SIGNAL(frameworksNotFound(QList<FrameworkCheckModel>)), this, SIGNAL(frameworksNotFound(QList<FrameworkCheckModel>)));
    connect(fwChecker, SIGNAL(finishedWithChecking()), this, SIGNAL(finishedWithChecking()));

    // destruct
    connect(fwChecker, SIGNAL(finishedWithChecking()), checkerBgThread, SLOT(quit()));
    connect(fwChecker, SIGNAL(finishedWithChecking()), this, SLOT(_checkerFinished()));

    fwChecker->moveToThread(checkerBgThread);
    checkerBgThread->start();
}

void AvailableFrameworkChecker::_checkerFinished()
{
    if(this->sender() != NULL) {
        delete this->sender();
    }
}
