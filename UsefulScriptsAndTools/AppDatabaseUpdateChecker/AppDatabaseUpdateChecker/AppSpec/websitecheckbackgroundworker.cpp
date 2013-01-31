#include "websitecheckbackgroundworker.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QDateTime>

WebsiteCheckBackgroundWorker::WebsiteCheckBackgroundWorker(QList<WebsiteListDatabaseItemModel> websiteListItemsToCheck, QObject *parent) :
    QObject(parent),
    _networkManager(NULL)
{
    _websiteListItemsToCheck = websiteListItemsToCheck;
}

void WebsiteCheckBackgroundWorker::startChecking()
{
    this->_currItemIdx = -1;

    if(_networkManager == NULL)
    {
        _networkManager = new QNetworkAccessManager(this);

        connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(replyFinished(QNetworkReply*)));
    }

    this->startCheckingTheNextItem();
}

void WebsiteCheckBackgroundWorker::startCheckingTheNextItem()
{
    this->_currItemIdx++;

    if(this->_currItemIdx < this->_websiteListItemsToCheck.size()) {
        WebsiteListDatabaseItemModel currItemModel = this->_websiteListItemsToCheck[this->_currItemIdx];
        Q_EMIT startedCheckingItem(currItemModel.getAppName(), currItemModel.getUrl());
        _networkManager->get(QNetworkRequest(QUrl(currItemModel.getUrl())));
    }
    else {
        Q_EMIT finishedCompletely();
    }
}

void WebsiteCheckBackgroundWorker::replyFinished(QNetworkReply *theReply)
{
    QByteArray data = theReply->readAll();
    QString siteContent(data);

    //
    // simplify it to remove new-line and extra spaces etc
    siteContent = siteContent.simplified();

    WebsiteListDatabaseItemModel currItemModel = this->_websiteListItemsToCheck[this->_currItemIdx];
    this->_resultWebsiteListItems.append(WebsiteListDatabaseItemModel(currItemModel.getAppName(), currItemModel.getUrl(), currItemModel.getDownloadUrl(), siteContent, QString("%1").arg(QDateTime::currentDateTime().toTime_t()), QDateTime::currentDateTime().toString("yyyy.MM.dd - hh:mm:s"), currItemModel.getCheckSectionBegin(), currItemModel.getCheckSectionEnd(), currItemModel.getNote()));

    Q_EMIT finishedCheckingItem(currItemModel.getAppName(), currItemModel.getUrl());

    this->startCheckingTheNextItem();
}

QList<WebsiteListDatabaseItemModel> WebsiteCheckBackgroundWorker::getResultWebsiteListItems() const
{
    return _resultWebsiteListItems;
}
