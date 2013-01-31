#ifndef WEBSITECHECKBACKGROUNDWORKER_H
#define WEBSITECHECKBACKGROUNDWORKER_H

#include <QObject>
#include <QList>

#include "Database/websitelistdatabaseitemmodel.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class WebsiteCheckBackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit WebsiteCheckBackgroundWorker(QList<WebsiteListDatabaseItemModel> websiteListItemsToCheck, QObject *parent = 0);

    void startChecking();

    QList<WebsiteListDatabaseItemModel> getResultWebsiteListItems() const;

Q_SIGNALS:
    void startedCheckingItem(QString appname, QString url);
    void finishedCheckingItem(QString appname, QString url);
    void finishedCompletely();

private Q_SLOTS:
    void replyFinished(QNetworkReply *theReply);

private:
    void startCheckingTheNextItem();

private:
    QList<WebsiteListDatabaseItemModel> _websiteListItemsToCheck;
    int _currItemIdx;

    QList<WebsiteListDatabaseItemModel> _resultWebsiteListItems;

    QNetworkAccessManager* _networkManager;
};

#endif // WEBSITECHECKBACKGROUNDWORKER_H
