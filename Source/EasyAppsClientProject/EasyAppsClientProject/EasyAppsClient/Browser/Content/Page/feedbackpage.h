#ifndef FEEDBACKPAGE_H
#define FEEDBACKPAGE_H

#include <QWidget>
#include "Browser/Content/Page/browserpagebase.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class CommunicationDescriptionGateway;
class ContentCoverStatusMessageWidget;

class BetterTextEdit;
class CheckboxButton;
class WidgetAnimatedTwoSizeResizer;

namespace Ui {
    class feedbackPage;
}

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
}
}
}


class FeedbackPage : public BrowserPageWidgetBase
{
    Q_OBJECT

public:
    static QString getPageParameterForDefaultText(QString defaultFeedbackText);

public:
    explicit FeedbackPage(QString pageId, QWidget *parent = 0);
    ~FeedbackPage();

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
    Ui::feedbackPage *ui;
    BetterTextEdit *_feedbackTextEdit;
    CheckboxButton *_attachSystemInfoCheckboxButton;
    QTextEdit *_attachedSystemInfoSummaryTextEdit;
    WidgetAnimatedTwoSizeResizer *_attachedSystemInfoSummaryTextEditResizer;
    ContentCoverStatusMessageWidget *_successFeedbackStatusPresenterWidget;

    CommunicationDescriptionGateway*    communicationDescriptionGateway;
};

#endif // FEEDBACKPAGE_H
