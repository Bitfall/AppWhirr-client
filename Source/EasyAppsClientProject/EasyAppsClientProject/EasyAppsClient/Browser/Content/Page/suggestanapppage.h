#ifndef SUGGESTANAPPPAGECONTENTWIDGET_H
#define SUGGESTANAPPPAGECONTENTWIDGET_H

#include "Browser/Content/Page/browserpagebase.h"

class CommunicationDescriptionGateway;
class ContentCoverStatusMessageWidget;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
}
}
}

namespace Ui {
    class SuggestAnAppPageContentWidget;
}

class SuggestAnAppPage : public BrowserPageWidgetBase
{
    Q_OBJECT

public:
    explicit SuggestAnAppPage(QString pageId, QWidget *parent = 0);
    ~SuggestAnAppPage();

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

private Q_SLOTS:
    void _backButtonPressed();
    void _sendButtonPressed();
    void _authenticationChallengeHappened();
    void _requestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities);
    void _communicationError(QString error);
    void _requestFailed();

protected:
    void resizeEvent(QResizeEvent *e);

protected:
    // todo ezt majd cppbe csak most gyors heggesztes
    QList<QString>getSubpageRequestURLs() { QList<QString> a; return a; }

private:
    Ui::SuggestAnAppPageContentWidget *ui;
    ContentCoverStatusMessageWidget *_successFeedbackStatusPresenterWidget;

    CommunicationDescriptionGateway*    communicationDescriptionGateway;
};

#endif // SUGGESTANAPPPAGECONTENTWIDGET_H
