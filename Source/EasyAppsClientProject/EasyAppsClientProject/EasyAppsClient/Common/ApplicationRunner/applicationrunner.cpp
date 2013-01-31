#include "applicationrunner.h"

#include <QThreadPool>

#include "Common/Helpers/pathhelper.h"

#include "Application/globalobjectregistry.h"
#include "Browser/Notification/notificationmanager.h"

// background-workers
#include "background_applicationrunner.h"

// config
#define DEFAULT_RUN_DELAY_IN_QUEUE 5000
#define LONGER_RUN_DELAY_IN_QUEUE 7000

// ----------------------------------
// --- ApplicationRunner ---

ApplicationRunner::ApplicationRunner(bool isQueueMode, QObject *parent) :
    QObject(parent),
    _isQueueMode(isQueueMode)
{
    _delayStarterTimer = new QTimer(this);
    _delayStarterTimer->setSingleShot(true);
    _delayStarterTimer->setInterval(DEFAULT_RUN_DELAY_IN_QUEUE);
    connect(_delayStarterTimer, SIGNAL(timeout()), this, SLOT(_tryToRunTheNextItemInTheQueue()));
    _delayStarterTimer->stop();

//    connect(this, SIGNAL(successWithApp(QString)), this, SLOT(_successWithApp(QString)));
//    connect(this, SIGNAL(failedWithApp(QString,QString)), this, SLOT(_failedWithApp(QString,QString)));
}

void ApplicationRunner::runSingleApplication(QString absoluteExecutablePath, QString absoluteWorkingDirPath, QString appName, QStringList appArgs)
{
    // Check if it exists
    DLog("Start application with path: " << absoluteExecutablePath);
//    if(!PathHelper::isFileExecutable(absoluteExecutablePath)) {
//        WLog("Invalid application path");
////        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application not found: ") + absoluteExecutablePath, 10000, NotificationManager::ErrorPriority);
//        Q_EMIT failedWithApp(appName, tr("Application executable not found."), this);
//    }

    Background_ApplicationRunner *worker = new Background_ApplicationRunner(absoluteExecutablePath, absoluteWorkingDirPath, appName, appArgs);
    connect(worker, SIGNAL(successWithApp(QString)), this, SLOT(_successWithApp(QString)));
    connect(worker, SIGNAL(failedWithError(QString,QString)), this, SLOT(_failedWithApp(QString,QString)));
    QThreadPool::globalInstance()->start(worker);
}

void ApplicationRunner::addApplicationToRunQueue(QString absoluteExecutablePath, QString absoluteWorkingDirPath, QString appName)
{
    this->_queueItems.enqueue(ApplicationRunnerQueueItem(absoluteExecutablePath, absoluteWorkingDirPath, appName));
}

bool ApplicationRunner::isRunQueueEmpty() const
{
    return this->_queueItems.isEmpty();
}

int ApplicationRunner::getAppCountInQueue() const {
    return this->_queueItems.size();
}

bool ApplicationRunner::getIsQueueMode() const
{
    return this->_isQueueMode;
}

void ApplicationRunner::runApplicationsInQueueWithDelay()
{   
    if(this->_queueItems.isEmpty()) {
        // no items added
        DLog("No items added to the app-runner queue. Won't start any apps.");
        // and start the timer
//        this->_delayStarterTimer->stop();
        Q_EMIT finishedWithQueueAndSafeToDelete(this);
    }
    else {
        _startedQueueItemCnt = 0;
        // start the first one now
        this->_tryToRunTheNextItemInTheQueue();
        // and start the timer
//        this->_delayStarterTimer->start(DEFAULT_RUN_DELAY_IN_QUEUE);
    }
}

void ApplicationRunner::clearApplicationsFromQueue()
{
    this->_delayStarterTimer->stop();
    this->_queueItems.clear();
    Q_EMIT finishedWithQueueAndSafeToDelete(this);
}

void ApplicationRunner::_tryToRunTheNextItemInTheQueue()
{
    if(!_queueItems.isEmpty()) {
        ApplicationRunnerQueueItem queueItem = _queueItems.dequeue();
        ++_startedQueueItemCnt;
        runSingleApplication(queueItem.getAbsoluteExecutablePath(), queueItem.getAbsoluteExecutableWorkingDirectoryPath(), queueItem.getAppName());
    }

    else {
        // finished, no more items in the queue
        this->_delayStarterTimer->stop();
        Q_EMIT finishedWithQueueAndSafeToDelete(this);
    }
}

void ApplicationRunner::_successWithApp(QString appName)
{
    Q_EMIT successWithApp(appName, this);
    if(_isQueueMode) {
        int delayMilliseconds = DEFAULT_RUN_DELAY_IN_QUEUE;
        if( (_startedQueueItemCnt % 2) == 1) {
            delayMilliseconds = LONGER_RUN_DELAY_IN_QUEUE;
        }
        this->_delayStarterTimer->start(delayMilliseconds);
    }
}

void ApplicationRunner::_failedWithApp(QString appName, QString errorMessage)
{
    Q_EMIT failedWithApp(appName, errorMessage, this);
    if(_isQueueMode) {
        int delayMilliseconds = DEFAULT_RUN_DELAY_IN_QUEUE;
        if( (_startedQueueItemCnt % 2) == 1) {
            delayMilliseconds = LONGER_RUN_DELAY_IN_QUEUE;
        }
        this->_delayStarterTimer->start(delayMilliseconds);
    }
}
