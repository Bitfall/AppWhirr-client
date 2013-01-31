#include "databaseserver.h"

#include <QThread>
#include <QMutex>
#include <QMetaType>

#include "Common/Database/databaseworkperformerbase.h"

DatabaseServer::DatabaseServer(DatabaseServerDelegate *delegate, QObject *parent) :
    QObject(parent),
    _delegate(delegate),
    _isStarted(false),
    _serverThread(0)
{
    this->_registerRequiredMetaTypes();
}

// -------------------------
// --- server management ---

void DatabaseServer::startServer(QString databasePath)
{
    QMutex mutex;
    mutex.lock();

    if(this->_isStarted) {
        mutex.unlock();
        return;
    }

    DatabaseWorkPerformerBase *worker = _delegate->createDatabaseWorkerWithDatabaseAtPath(databasePath);

    // worker - server communication connections
    connect(this, SIGNAL(_addInitializeToWorkerQueue()), worker, SLOT(openAndInitialize()));
    connect(this, SIGNAL(_addShutDownToWorkerQueue()), worker, SLOT(closeAndShutDown()));
    connect(this, SIGNAL(_addThisWorkToWorkerQueue(DatabaseWorkAgentInterface*)), worker, SLOT(performThisWork(DatabaseWorkAgentInterface*)));

    connect(worker, SIGNAL(initialized()), this, SLOT(_workerInitialized()));
    connect(worker, SIGNAL(shutDownHappened()), this, SLOT(_workerShutDownHappened()));
    // this will be passed-over
    connect(worker, SIGNAL(finishedWithWork(DatabaseWorkAgentInterface*,bool)), this, SIGNAL(finishedWithWorkAgent(DatabaseWorkAgentInterface*,bool)));

    // server thread creation and connections
//    if(_serverThread == NULL) {
        _serverThread = new QThread;
        connect(_serverThread, SIGNAL(started()), this, SLOT(_serverThreadStarted()));
        connect(_serverThread, SIGNAL(finished()), this, SLOT(_serverThreadFinished()));
//    }

    worker->moveToThread(_serverThread);
    _serverThread->start();
    DLog("--server thread initialized--");

    this->_isStarted = true;
    mutex.unlock();
}

void DatabaseServer::addStopServerToQueue()
{
    DLog("--stop server requested--");
    Q_EMIT _addShutDownToWorkerQueue();
}

void DatabaseServer::stopServerImmediatelly()
{
    DLog("--stop server immediatelly--");

    QMutex mutex;
    mutex.lock();

    if(!this->_isStarted) {
        mutex.unlock();
        return;
    }

    DLog("Worker is not stopped correctly");
    this->_serverThread->quit();

    this->_isStarted = false;
    mutex.unlock();
}


// --------------------------
// --- server state slots ---

void DatabaseServer::_serverThreadStarted()
{
    DLog("--server thread started--");

    // start to initialize the worker
    Q_EMIT _addInitializeToWorkerQueue();
}

void DatabaseServer::_serverThreadFinished()
{
    DLog("--server thread finished--");
}

void DatabaseServer::_workerInitialized()
{
    DLog("--worker initialized--");

    // worker initialized - server is ready
    Q_EMIT serverStarted();
}

void DatabaseServer::_workerShutDownHappened()
{
    DLog("--worker shutdown--");

    QMutex mutex;
    mutex.lock();

    if(!this->_isStarted) {
        mutex.unlock();
        return;
    }

    this->_serverThread->quit();

    this->_isStarted = false;
    mutex.unlock();
}


void DatabaseServer::_registerRequiredMetaTypes()
{
    qRegisterMetaType<DatabaseWorkAgentInterface*>("DatabaseWorkAgentInterface*");
}

void DatabaseServer::addThisWorkAgentToQueue(DatabaseWorkAgentInterface *workAgent)
{
    Q_EMIT _addThisWorkToWorkerQueue(workAgent);
}
