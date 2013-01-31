#ifndef LOCALAPPDATABASESERVER_H
#define LOCALAPPDATABASESERVER_H

#include <QObject>
#include <QMap>

#include "databaseserverdelegate.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class DatabaseWorkAgentInterface;


/*!

  Informations:\n
      * The whole server communication is asynchronous (except some server-management methods: currently only stopServerImmediatelly is not async)\n
      ** But the server runs and communicatates with the database in 1 thread, and the given operations are stored and processed by an event-queue, so the order or the results will be the order of the given operations\n

  @note Usage:\n
      * You have to implement the DatabaseServerDelegate and give it in the constuctor of the DatabaseServer\n
      * You can start a query by calling addThisWorkAgentToQueue() with a work-agent in the heap (create it with "new ...Agent()")\n
      ** This will start the query in the server in async mode\n
      * [!] So before you start a query connect to finishedWithWorkAgent() signal\n
      ** This signal will be emitted with the result(s)\n
      * [!] you HAVE TO free-up the work-agent after the finishedWithWorkAgent() signal emitted, because the server won't!!\n
      ** you can delete the work-agent safely after the finishedWithWorkAgent() signal emitted -> so usually relese it in your connected slot\n
*/

class DatabaseServer : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseServer(DatabaseServerDelegate *delegate, QObject *parent = 0);

    // -------------------------
    // --- server management ---

    void startServer(QString databasePath);
    void addStopServerToQueue();
    void stopServerImmediatelly();

    // you have to manage the work-agent's life-cycle:
    //  you give it in here, and you will get the result when finishedWithWorkAgent() is emitted
    //  at this point you can delete the work-agent
    //
    // [!] you HAVE TO free-up the work-agent after the finishedWithWorkAgent() signal emitted, because the server won't!!
    void addThisWorkAgentToQueue(DatabaseWorkAgentInterface *workAgent);

Q_SIGNALS: // public server signals
    void serverStarted();
    void serverStopped();

    void finishedWithWorkAgent(DatabaseWorkAgentInterface *workAgent, bool isQuerySuccess);

Q_SIGNALS: // PRIVATE, server-worker communication signals
    void _addShutDownToWorkerQueue();
    void _addInitializeToWorkerQueue();
    void _addThisWorkToWorkerQueue(DatabaseWorkAgentInterface *workAgent);

private Q_SLOTS:
    // --------------------------
    // --- server state slots ---

    // thread
    void _serverThreadStarted();
    void _serverThreadFinished();
    // worker
    void _workerInitialized();
    void _workerShutDownHappened();

private:
    void _registerRequiredMetaTypes();

private:
    DatabaseServerDelegate *_delegate;
    bool _isStarted;
    QThread *_serverThread;
};

#endif // LOCALAPPDATABASESERVER_H
