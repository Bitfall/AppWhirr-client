#ifndef UNINSTALLCONFIRMANDFEEDBACKPAGE_H
#define UNINSTALLCONFIRMANDFEEDBACKPAGE_H

#include "browserpagebase.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

class CommunicationDescriptionGateway;
class ContentCoverStatusMessageWidget;

class BetterTextEdit;
class CheckboxButton;
class WidgetAnimatedTwoSizeResizer;

class ApplicationRunner;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
}
}
}


class UninstallConfirmAndFeedbackPage : public BrowserPageWidgetBase
{
    Q_OBJECT

public:
    explicit UninstallConfirmAndFeedbackPage(QString pageId, QWidget *parent = 0);
    ~UninstallConfirmAndFeedbackPage();

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

    void _startUninstallButtonClicked();

    void _uninstallerTmpFolderDeleted();
    void _uninstallerCopyToTmpFinished();
    void _uninstallerAppRunSuccess(QString appName, ApplicationRunner *source);
    void _uninstallerAppRunError(QString appName, QString errorMessage, ApplicationRunner *source);
    void _uninstallExecutionError(QString errorMessage);

protected:
    void resizeEvent(QResizeEvent *e);

protected:
    // todo ezt majd cppbe csak most gyors heggesztes
    QList<QString>getSubpageRequestURLs() { QList<QString> a; return a; }
private:
    QPushButton *_sendButton;
    QPushButton *_startUninstallButton;
    BetterTextEdit *_feedbackTextEdit;
    CheckboxButton *_attachSystemInfoCheckboxButton;
    QTextEdit *_attachedSystemInfoSummaryTextEdit;
    WidgetAnimatedTwoSizeResizer *_attachedSystemInfoSummaryTextEditResizer;
    ContentCoverStatusMessageWidget *_successFeedbackStatusPresenterWidget;

    CommunicationDescriptionGateway*    communicationDescriptionGateway;

    ApplicationRunner *_appRunner;
};

#endif // UNINSTALLCONFIRMANDFEEDBACKPAGE_H
