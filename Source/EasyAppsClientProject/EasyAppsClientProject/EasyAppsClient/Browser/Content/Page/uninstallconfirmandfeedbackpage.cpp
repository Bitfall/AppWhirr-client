#include "uninstallconfirmandfeedbackpage.h"

#include "Application/Widgets/incontentnavigationwidget.h"
#include "Common/Widgets/contentcoverstatusmessagewidget.h"

#include "Common/Widgets/bettertextedit.h"
#include "Common/Widgets/checkboxbutton.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QDesktopServices>
#include <QThreadPool>
#include <QApplication>

// helper
#include "Common/widgetanimatedtwosizeresizer.h"
#include "Common/ApplicationRunner/applicationrunner.h"
#include "Common/Runnables/runnable_copydirectory.h"
#include "Common/Runnables/runnable_deletedirectoryrecursively.h"

// menu
#include "Common/Menu/menumanager.h"
#include "Settings/menuitemidcenter.h"
#include "Common/Page/pagenavigationmanager.h"

// config
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/uifactory.h"
#include "Application/appspecificpathhelper.h"
#include "Common/Helpers/pathhelper.h"

// nofig
#include "Browser/Notification/notificationmanager.h"

// system profile
#include "Common/System/systemprofiler.h"

// Network
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Common/Security/authorization.h"


#define PAGE_OPTION_PREFIX_DEFAULTTEXT  QString("deftext:")

UninstallConfirmAndFeedbackPage::UninstallConfirmAndFeedbackPage(QString pageId, QWidget *parent) :
    BrowserPageWidgetBase(pageId, parent),
    communicationDescriptionGateway(NULL)
{
//    ui->webView->setUrl(QUrl("http://appwhirr.uservoice.com/forums/130853-feedback"));

//    http://appwhirr.uservoice.com/forums/130853-feedback
    _successFeedbackStatusPresenterWidget = new ContentCoverStatusMessageWidget(300, 4000);
    _successFeedbackStatusPresenterWidget->setParent(this);
    _successFeedbackStatusPresenterWidget->hide();

    _appRunner = new ApplicationRunner(false, this);
    connect(_appRunner, SIGNAL(successWithApp(QString,ApplicationRunner*)), this, SLOT(_uninstallerAppRunSuccess(QString,ApplicationRunner*)));
    connect(_appRunner, SIGNAL(failedWithApp(QString,QString,ApplicationRunner*)), this, SLOT(_uninstallerAppRunError(QString,QString,ApplicationRunner*)));
}

UninstallConfirmAndFeedbackPage::~UninstallConfirmAndFeedbackPage()
{
}

void UninstallConfirmAndFeedbackPage::initializePageContent()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    QVBoxLayout *contentLayout = new QVBoxLayout;
    mainLayout->addLayout(contentLayout);
    QVBoxLayout *feedbackTextEditHolderLayout = new QVBoxLayout;
    contentLayout->addLayout(feedbackTextEditHolderLayout);
    _sendButton = new QPushButton;
    contentLayout->addWidget(_sendButton);
    feedbackTextEditHolderLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    _startUninstallButton = new QPushButton;
    contentLayout->addWidget(_startUninstallButton);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentLayout->addSpacerItem(verticalSpacer);

    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Back"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->insertWidget(0, inContentNavigationWidget);

    contentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));
    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    _feedbackTextEdit = new BetterTextEdit;
    _feedbackTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/Default/LightGray/style"));
    _feedbackTextEdit->setPlaceholderText(tr("We would like to hear your voice! Before you uninstall AppWhirr please send us your concerns, and why have you decided to uninstall AppWhirr.\n\nWe really appriciate every feedback and we're constantly working on improving our system.\n\nThank you!"));
    feedbackTextEditHolderLayout->addWidget(_feedbackTextEdit);

    feedbackTextEditHolderLayout->addSpacing(10);
    _attachSystemInfoCheckboxButton = new CheckboxButton(tr("Attach information to hep us solve errors quicker."));
    _attachSystemInfoCheckboxButton->setChecked(true);
    _attachSystemInfoCheckboxButton->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
    _attachSystemInfoCheckboxButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    feedbackTextEditHolderLayout->addWidget(_attachSystemInfoCheckboxButton);

    feedbackTextEditHolderLayout->addSpacing(3);
    PlatformFlagsHelper::SummedSinglePlatformInformations *currSystemPlatformInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    QString currPlatformInfoPrintableText = "Undefined OS";
    if(currSystemPlatformInfo != NULL) {
        currPlatformInfoPrintableText = currSystemPlatformInfo->getAsPrintableText();
    }

    _attachedSystemInfoSummaryTextEdit = new QTextEdit;
    _attachedSystemInfoSummaryTextEdit->setHtml(tr("We will attach these informations:<br />- Information about your system: %1<br />").arg(currPlatformInfoPrintableText));
    _attachedSystemInfoSummaryTextEdit->setReadOnly(true);
    _attachedSystemInfoSummaryTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/Default/LightGray/style"));
    feedbackTextEditHolderLayout->addWidget(_attachedSystemInfoSummaryTextEdit);

    _attachedSystemInfoSummaryTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    _attachedSystemInfoSummaryTextEditResizer = new WidgetAnimatedTwoSizeResizer(_attachedSystemInfoSummaryTextEdit, 500, QSize(0, 0), QSize(100, 100), true, false, true, WidgetAnimatedTwoSizeResizer::WRM_SetOnlyMaximumSize, this);
    connect(_attachSystemInfoCheckboxButton, SIGNAL(toggled(bool)), _attachedSystemInfoSummaryTextEditResizer, SLOT(switchState(bool)));


    feedbackTextEditHolderLayout->addSpacing(10);

    _sendButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    _sendButton->setFlat(true);
    _sendButton->setText(tr("Send feedback"));
    connect(_sendButton, SIGNAL(clicked()), this, SLOT(_sendButtonPressed()));

    _startUninstallButton->setStyleSheet(guiConfManager->getStyleForId("Button/DangerousAction/style"));
    _startUninstallButton->setFlat(true);
    _startUninstallButton->setText(tr("Start AppWhirr uninstall"));
    connect(_startUninstallButton, SIGNAL(clicked()), this, SLOT(_startUninstallButtonClicked()));
}

void UninstallConfirmAndFeedbackPage::pagePushWillHappen()
{

}

void UninstallConfirmAndFeedbackPage::pageWillAppear(QString param)
{
    if(!param.isEmpty()) {
        if(param.startsWith(PAGE_OPTION_PREFIX_DEFAULTTEXT)) {
            _feedbackTextEdit->setText(param.mid(PAGE_OPTION_PREFIX_DEFAULTTEXT.length()));
        }
    }
}

void UninstallConfirmAndFeedbackPage::pageDidAppear()
{

}

void UninstallConfirmAndFeedbackPage::pagePopWillHappen()
{

}

void UninstallConfirmAndFeedbackPage::pageWillDisappear()
{

}

void UninstallConfirmAndFeedbackPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

void UninstallConfirmAndFeedbackPage::_sendButtonPressed() {
    DLog("Feedback content: ") << _feedbackTextEdit->toPlainText();

    if(_feedbackTextEdit->getNonPlaceholderText().isEmpty()) {
        DLog("Empty feedback, do nothing.");
        _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("You should write your feedback before sending it."));
        return;
    }

    disconnect(Authorization::sharedAuthorization());
    this->_sendButton->setDisabled(true);

    if (communicationDescriptionGateway) {
        disconnect(communicationDescriptionGateway);
        AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    }

    communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(communicationDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_authenticationChallengeHappened()));
    connect(communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_requestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_communicationError(QString)));

    QString feedbackText = QString("[uninstall feedback]\n") + _feedbackTextEdit->toPlainText();
    if(_attachSystemInfoCheckboxButton->getIsChecked()) {
        PlatformFlagsHelper::SummedSinglePlatformInformations *currSystemPlatformInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
        QString currPlatformInfoPrintableText = "Undefined OS";
        if(currSystemPlatformInfo != NULL) {
            currPlatformInfoPrintableText = currSystemPlatformInfo->getAsPrintableText();
        }
        feedbackText += "\n\n#Platform info: " + currPlatformInfoPrintableText;
    }
    communicationDescriptionGateway->requestForUserFeedback(feedbackText);
}

void UninstallConfirmAndFeedbackPage::_authenticationChallengeHappened() {
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_sendButtonPressed()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginCommunicationError(QString)), this, SLOT(_communicationError(QString)));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(_requestFailed()));
}

void UninstallConfirmAndFeedbackPage::_requestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities) {
    this->_sendButton->setDisabled(false);
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Thank you for your feedback!"));
    _feedbackTextEdit->clear();
}

void UninstallConfirmAndFeedbackPage::_communicationError(QString error) {
    this->_requestFailed();
}

void UninstallConfirmAndFeedbackPage::_requestFailed() {
    this->_sendButton->setDisabled(false);
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Failed to send it. We're truly ashamed, sorry..."));
}

void UninstallConfirmAndFeedbackPage::resizeEvent(QResizeEvent *e)
{
    BrowserPageWidgetBase::resizeEvent(e);

    _successFeedbackStatusPresenterWidget->setGeometryToThis(this->geometry());
}

void UninstallConfirmAndFeedbackPage::_startUninstallButtonClicked()
{
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Preparing the uninstaller..."));
    // copy the AWUninstaller
//    _appRunner->runSingleApplication();
//    AppSpecificPathHelper::getUninstallerExecutablePath();

    DLog("TMP uninst. folder: ") << AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTempForUninstaller();

    Runnable_DeleteDirectoryRecursively *deleteTmpUninstallerFolderWorker = new Runnable_DeleteDirectoryRecursively(AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTempForUninstaller());
    connect(deleteTmpUninstallerFolderWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_uninstallerTmpFolderDeleted()));
    connect(deleteTmpUninstallerFolderWorker, SIGNAL(failedWithError(QString)), this, SLOT(_uninstallerTmpFolderDeleted()));
    QThreadPool::globalInstance()->start(deleteTmpUninstallerFolderWorker);
}

void UninstallConfirmAndFeedbackPage::_uninstallerTmpFolderDeleted()
{
    QString uninstallerDirPath = AppSpecificPathHelper::getUninstallerDirPath();
    DLog("uninst.dir.path: ") << uninstallerDirPath;
    QString targetTmpDirPath = AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTempForUninstaller();
#ifdef Q_OS_WIN
    bool isCopyOnlyContentsOfUninstDir = true;
#endif
#ifdef Q_OS_MAC
    bool isCopyOnlyContentsOfUninstDir = false;
#endif
    Runnable_CopyDirectory *uninstallerCopyWorker = new Runnable_CopyDirectory(uninstallerDirPath, targetTmpDirPath, isCopyOnlyContentsOfUninstDir, PathHelper::SHM_UseAsFile, true);
    connect(uninstallerCopyWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_uninstallerCopyToTmpFinished()));
    connect(uninstallerCopyWorker, SIGNAL(failedWithError(QString)), this, SLOT(_uninstallExecutionError(QString)));
    QThreadPool::globalInstance()->start(uninstallerCopyWorker);
}

void UninstallConfirmAndFeedbackPage::_uninstallerCopyToTmpFinished()
{
    QString uninstallerAppPathInTmp = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTempForUninstaller(), AppSpecificPathHelper::getUninstallerExecutableFileName());
    DLog("Uninstaller tmp path: ") << uninstallerAppPathInTmp;

    QString uninstallerAppWorkingDirInTmp = AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTempForUninstaller();

    QStringList argsForUninstaller;
    argsForUninstaller << "clientRootPath" << AppSpecificPathHelper::getClientExecutableDirectory();

    _appRunner->runSingleApplication(uninstallerAppPathInTmp, uninstallerAppWorkingDirInTmp, "AppWhirrUninstaller", argsForUninstaller);
}

void UninstallConfirmAndFeedbackPage::_uninstallerAppRunSuccess(QString appName, ApplicationRunner *source)
{
    // uninstaller started
    // quit from AppWhirr
    qApp->quit();
}

void UninstallConfirmAndFeedbackPage::_uninstallerAppRunError(QString appName, QString errorMessage, ApplicationRunner *source)
{
    this->_uninstallExecutionError(errorMessage);
}

void UninstallConfirmAndFeedbackPage::_uninstallExecutionError(QString errorMessage)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addDefaultErrorMessageNotificationWithTimeout(tr("Cannot run the Uninstaller. Error: %1").arg(errorMessage));
}
