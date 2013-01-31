#include "localmessagesender.h"

#include <QString>
#include <QByteArray>
#include <QLocalSocket>

//#include "../PathManager/pathmanager.h"

LocalMessageSender::LocalMessageSender(QString uniqueKey, unsigned int timeout) :
    QObject(0),
    uniqueKey(uniqueKey),
    timeout(timeout)
{
}

//LocalMessageSender::~LocalMessageSender() {
//    this->disconnect();
//}

void LocalMessageSender::sendSingleMessage(const QByteArray& message) {
    QLocalSocket* localSocket = new QLocalSocket(this);

    // Connect to the server
    localSocket->connectToServer(uniqueKey, QIODevice::WriteOnly);

    // Wait for connection estabilish
    if (!localSocket->waitForConnected(timeout)) {
        DLogS << "Failed to estabilish connection";

        Q_EMIT failed(tr("Cannot connect."));
        return;
    }

    localSocket->write(message);
    if (!localSocket->waitForBytesWritten(timeout)) {
        DLogS << "Failed to send message";

        Q_EMIT failed(tr("Cannot write to connection."));
        return;
    }

    // Disconnect from the server
    localSocket->disconnectFromServer();

    Q_EMIT this->finishedWithSuccess();
}
