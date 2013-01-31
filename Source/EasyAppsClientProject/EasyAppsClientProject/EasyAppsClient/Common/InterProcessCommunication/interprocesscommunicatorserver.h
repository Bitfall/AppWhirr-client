#ifndef INTERPROCESSCOMMUNICATORSERVER_H
#define INTERPROCESSCOMMUNICATORSERVER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QLocalServer;
class QTimer;
QT_END_NAMESPACE

//
// provides inter-process-communication - server-side
//  tries to provide a safe (error-proof) communication interface - the default QLocalServer and Client connection and message sending can fail from time to time - this server (and it's compatible client) tries to make it more error-proof
//  also it provides some further error handling with timeout interval between messages (see more below)
//
//  some specifications / notes:
//      * the related client has to send "still-processing" messages to this server from time-to-time if no message arrives for a while. This ment to close the communication even if the communication-client process (application) crashes
//      * this "still-processing" status message sending period has to be synchronized with this server (have to be less than timeoutBetweenMessagesInMilliseconds constructor parameter, but it's better to
//
class InterProcessCommunicatorServer : public QObject
{
    Q_OBJECT
public:
    explicit InterProcessCommunicatorServer(QString baseConnectionName, int timeoutBetweenMessagesInMilliseconds, QObject *parent = 0);
    virtual ~InterProcessCommunicatorServer();

private Q_SLOTS:
    void updaterMessageTimeout();
    void _newConnection();

private:
    QLocalServer *_localServer;
    QTimer *_waitingForMessageTimeoutTimer;
};

#endif // INTERPROCESSCOMMUNICATORSERVER_H
