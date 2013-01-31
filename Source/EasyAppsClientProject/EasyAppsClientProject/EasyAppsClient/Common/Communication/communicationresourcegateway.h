#ifndef COMMUNICATIONRESOURCEGATEWAY_H
#define COMMUNICATIONRESOURCEGATEWAY_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include "Common/Communication/basenetworkrequest.h"
#include <QUrl>

#include "communicationresourcegatewaydelegate.h"
#include "communicationresourcegatewaycachedelegate.h"

namespace AW {
namespace Common {
namespace Communication {

class CommunicationResourceGateway : public QObject
{
    Q_OBJECT

public:
    /*! Resource gateway

        @param gatewayCacheDelegate : if NULL no cache will be applied
    */
    explicit CommunicationResourceGateway(CommunicationResourceGatewayDelegate *gatewayDelegate, CommunicationResourceGatewayCacheDelegate *gatewayCacheDelegate = NULL, unsigned int maxParallelRequestsCount = 3, QObject *parent = 0);
    virtual ~CommunicationResourceGateway();

    /*! Adds a new resource URL to be loaded */
    /** After adding all the resources you should call the startRequests() method
        @warning It do not starts loading the resource.
      */
    void addResourceUrl(QUrl url);

    /*! Starts loading the requests */
    /**
      */
    void startRequests();

Q_SIGNALS:
    /*! Emits when a resource was retrieved*/
    /**
      */
    void resourceChunkReceivedForUrl(QUrl url, QByteArray resourceChunk, bool isBufferDownloadedResourceAndSendItWhenDownloadFinished);
    void resourceDownloadFailedForUrl(QUrl url);
    /*! Will be emitted when the given resource is 100% retrieved
    */
    void resourceDownloadFinishedWithSuccess(QUrl url);
    /*! Emits when all resources were retrieved */
    /** @warning You should only connect to this Q_SIGNAL, when all your resources is added to this class
      */
    void finishedWithAllRequests();

    void downloadProgressForUrl(int progress, QUrl url);

private Q_SLOTS:
    void requestFinished();
    void responseDataChunkAvailable(QByteArray response, bool isBufferDownloadedResourceAndSendItWhenDownloadFinished);
    void requestFailed();
    void downloadProgress(int progress);


    // ------------------------------
    // -- queue management
Q_SIGNALS:
    void __updateRequestQueueSignal();
private Q_SLOTS:
    /*! Starts new pending requests (until it hits max-parallel-requests-count) or emits \a finishedWithAllRequests() if no more pending requests found.
    */
    void updateRequestQueue();

private:
    QList<QUrl>                 _resources;
    QList<BaseNetworkRequest*>  _networkRequests;
    const quint32               _maxParallelRequestsCount;
//    quint32                     _activeParallelRequestsCount;

    CommunicationResourceGatewayDelegate *_gatewayDelegate;
    CommunicationResourceGatewayCacheDelegate *_gatewayCacheDelegate;
    NetworkRequestCacheDelegate *_requestCacheDelegate;
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // COMMUNICATIONRESOURCEGATEWAY_H
