#ifndef LICENCEPRESENTERPAGE_H
#define LICENCEPRESENTERPAGE_H

#include "Common/Page/pagewidgetbase.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QNetworkReply;
QT_END_NAMESPACE

#include "Application/Communication/Gateway/communicationdescriptiongateway.h"

class LicencePresenterPage : public PageWidgetBase
{
    Q_OBJECT

public:
    explicit LicencePresenterPage(QString pageId, QString relatedAppId, QWidget *parent = 0);

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

private Q_SLOTS:
    void relatedMenuItemActivated(QString menuItemId);

    void _backButtonPressed();

    void networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities);
    void failedToGetDescription(QString errorMessage);

private:
    QString _relatedAppId;
    QTextEdit *_licenceTextEdit;

    CommunicationDescriptionGateway*    communicationDescriptionGateway;
};

#endif // LICENCEPRESENTERPAGE_H
