#ifndef COMMUNICATIONRESOURCEGATEWAY_H
#define COMMUNICATIONRESOURCEGATEWAY_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include "Common/Communication/basenetworkrequest.h"

class CommunicationResourceGateway : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationResourceGateway(unsigned int numberOfRequestsParalell = 5);
    virtual ~CommunicationResourceGateway();

    /*! Adds a new resource URL to be loaded */
    /** After adding all the resources you should call the startRequests() method
        @warning It do not starts loading the resource.
      */
    void addResourceURL(const QString& URL);

    /*! Starts loading the requests */
    /**
      */
    void startRequests();

Q_SIGNALS:
    /*! Emits when a resource was retrieved*/
    /**
      */
    void resourceReceivedForURL(const QString& URL, const QByteArray& response);
    void resourceDownloadFailedForUrl(const QString &URL);
    /*! Emits when all resources were retrieved */
    /** @warning You should only connect to this Q_SIGNAL, when all your resources is added to this class
      */
    void finished();

    void downloadProgressForUrl(int progress, QString url);

private Q_SLOTS:
    void requestFinished();
    void requestReceived(const QByteArray& response);
    void requestFailed();
    void downloadProgress(int progress);

private:
    bool isResourceCacheable(const QString& URL);

    QList<QString>              resources;
    QList<BaseNetworkRequest*>  networkRequests;
    unsigned int                numberOfRequestsParalell;
};

#endif // COMMUNICATIONRESOURCEGATEWAY_H
