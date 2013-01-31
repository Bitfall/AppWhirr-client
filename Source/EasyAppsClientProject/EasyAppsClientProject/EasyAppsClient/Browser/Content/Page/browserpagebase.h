#ifndef BROWSERPAGEINTERFACE_H
#define BROWSERPAGEINTERFACE_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QNetworkConfigurationManager>

#include "Common/Menu/menuitemwidget.h"
#include "Common/Page/pagewidgetbase.h"


class CommunicationDescriptionGateway;
class CoverWithCustomWidget;

class BrowserPageWidgetBase : public PageWidgetBase
{
    Q_OBJECT

public:
    explicit BrowserPageWidgetBase(QString pageId, QWidget *parent = 0);
    virtual ~BrowserPageWidgetBase();

    virtual void initializePageContent() = 0;
    virtual void pagePushWillHappen() = 0;
    virtual void pageWillAppear(QString param) = 0;
    virtual void pageDidAppear() = 0;
    virtual void pagePopWillHappen() = 0;
    virtual void pageWillDisappear() = 0;    

public Q_SLOTS:
    virtual void initializeRequests();

Q_SIGNALS:
    void pageCommunicationFinished();

protected:
    void resizeEvent(QResizeEvent *e);

protected:
    CommunicationDescriptionGateway*    communicationDescriptionGateway;
//    CommunicationResourceGateway*       communicationResourceGateway;

    /*! Gethers the request URLs, what the subpages will request for
    */
    virtual QList<QString> getSubpageRequestURLs() = 0;

    bool                                checkForNetworkConnection;  // Checks the network accessibility
    bool                                isShowingErrorPage;
    QNetworkConfigurationManager*       networkConfigurationManager;

private:
    CoverWithCustomWidget *_networkUnreachableCoverWidget;
};

#endif // BROWSERPAGEINTERFACE_H
