#ifndef SIMPLESSLCLIENT_H
#define SIMPLESSLCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QString>

class SimpleSslClient : public QObject
{
    Q_OBJECT
public:
    SimpleSslClient(QObject* parent = 0);
    void startRequest(QString hostName, quint16 port);
public Q_SLOTS:
    // handle the signal of QSslSocket.encrypted()
    void connectionEstablished();
    // handle the signal of QSslSocket.sslErrors()
    void errorOccured(const QList<QSslError> &error);

private Q_SLOTS:
    void _readyRead();

private:
    QSslSocket client;
};

#endif // SIMPLESSLCLIENT_H
