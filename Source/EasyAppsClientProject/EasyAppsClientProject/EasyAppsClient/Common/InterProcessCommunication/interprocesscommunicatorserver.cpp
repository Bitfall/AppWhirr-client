#include "interprocesscommunicatorserver.h"

#include <QLocalServer>
#include <QTimer>

#ifdef Q_WS_MAC
#include <QFile>
#endif

InterProcessCommunicatorServer::InterProcessCommunicatorServer(QString baseConnectionName, int timeoutBetweenMessagesInMilliseconds, QObject *parent) :
    QObject(parent),
    _localServer(NULL)
{
    _localServer = new QLocalServer(this);

    QString listenIdentifier = baseConnectionName;

#ifdef Q_WS_MAC
    FLAG_FOR_REVIEW_WITH_HINT("This should work even if the listenIdentifier is not a file! (?)");
    // On Unix if the server crashes without closing listen will fail with AddressInUseError.
    // To create a new server the file should be removed.
    QFile::remove(listenIdentifier);
#endif

#ifdef Q_WS_WIN
    // Todo:
    // On Windows two local servers can listen to the same pipe at the same time,
    // but any connections will go to one of the servers.
    FLAG_FOR_REVIEW_WITH_HINT("Maybe this is not a problem, on Windows, but review it.");
#endif

    _waitingForMessageTimeoutTimer = new QTimer(this);
    _waitingForMessageTimeoutTimer->setSingleShot(true);
    _waitingForMessageTimeoutTimer->setInterval(timeoutBetweenMessagesInMilliseconds);
    connect(_waitingForMessageTimeoutTimer, SIGNAL(timeout()), this, SLOT(updaterMessageTimeout()));
    _waitingForMessageTimeoutTimer->stop();

    connect(_localServer, SIGNAL(newConnection()), this, SLOT(_newConnection()));
    if (!_localServer->listen(listenIdentifier)) {
        WLog("Failed to create server for installation");

        _localServer->close();
        _localServer->disconnect();
        AW_QOBJECT_SAFE_DELETE(_localServer);
    }
}

InterProcessCommunicatorServer::~InterProcessCommunicatorServer() {
    if (_localServer) {
        _localServer->close();
        _localServer->disconnect();
        AW_QOBJECT_SAFE_DELETE(_localServer);
    }
}

void InterProcessCommunicatorServer::_newConnection()
{

}

void InterProcessCommunicatorServer::updaterMessageTimeout()
{

}
