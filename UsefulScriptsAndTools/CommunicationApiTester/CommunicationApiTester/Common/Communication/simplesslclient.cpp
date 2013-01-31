#include "simplesslclient.h"

#include <QByteArray>
#include <QList>
#include <QSslCertificate>
#include <QString>

SimpleSslClient::SimpleSslClient(QObject* parent) :
    QObject(parent)
{
    connect(&client, SIGNAL(encrypted()), this, SLOT(connectionEstablished()));
    connect(&client, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(errorOccured(const QList<QSslError> &)));
    connect(&client, SIGNAL(readyRead()), this, SLOT(_readyRead()));
}

void SimpleSslClient::errorOccured(const QList<QSslError> & sslErrors)
{
    DLog("SSL errors found.");

    for(int i = 0; i < sslErrors.size(); ++i)
    {
        DLog("* SSL Error: ") << sslErrors[i].errorString();
    }

  // simply ignore the errors
  // it should be very careful when ignoring errors
//  client.ignoreSslErrors();
}

void SimpleSslClient::connectionEstablished()
{
    DLog("Connection established!");

    // get the peer's certificate
    QSslCertificate cert = client.peerCertificate();
    // write on the SSL connection
    client.write("Hello, world", 13);
}

void SimpleSslClient::startRequest(QString hostName, quint16 port)
{
    DLog("Start to connect: ") << hostName;
    client.connectToHostEncrypted(hostName, port);
}

void SimpleSslClient::_readyRead()
{
    DLog("Ready read.");
}
