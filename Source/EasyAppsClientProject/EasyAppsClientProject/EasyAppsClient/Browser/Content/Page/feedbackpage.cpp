#include "feedbackpage.h"
#include "ui_feedbackpage.h"

#include "Application/Widgets/incontentnavigationwidget.h"
#include "Common/Widgets/contentcoverstatusmessagewidget.h"

#include "Common/Widgets/bettertextedit.h"
#include "Common/Widgets/checkboxbutton.h"
#include <QTextEdit>

// helper
#include "Common/widgetanimatedtwosizeresizer.h"

// menu
#include "Common/Menu/menumanager.h"
#include "Settings/menuitemidcenter.h"
#include "Common/Page/pagenavigationmanager.h"

// config
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/uifactory.h"

// system profile
#include "Common/System/systemprofiler.h"

// Network
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Common/Security/authorization.h"


#define PAGE_OPTION_PREFIX_DEFAULTTEXT  QString("deftext:")

FeedbackPage::FeedbackPage(QString pageId, QWidget *parent) :
    BrowserPageWidgetBase(pageId, parent),
    ui(new Ui::feedbackPage),
    communicationDescriptionGateway(NULL)
{
    ui->setupUi(this);


//    ui->webView->setUrl(QUrl("http://appwhirr.uservoice.com/forums/130853-feedback"));

//    http://appwhirr.uservoice.com/forums/130853-feedback
    _successFeedbackStatusPresenterWidget = new ContentCoverStatusMessageWidget(300, 4000);
    _successFeedbackStatusPresenterWidget->setParent(this);
    _successFeedbackStatusPresenterWidget->hide();
}

FeedbackPage::~FeedbackPage()
{
    delete ui;
}

void FeedbackPage::initializePageContent()
{
    this->setContentsMargins(0, 0, 0, 0);
    ui->mainLayout->setContentsMargins(0, 0, 0, 0);
    ui->mainLayout->setSpacing(3);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Options"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    ui->mainLayout->insertWidget(0, inContentNavigationWidget);

    ui->contentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));
    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    _feedbackTextEdit = new BetterTextEdit;
    _feedbackTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/Default/LightGray/style"));
    _feedbackTextEdit->setPlaceholderText(tr("We are constantly working to make AppWhirr better every day. Feel free to share your thoughts with us."));
    ui->feedbackTextEditHolderLayout->addWidget(_feedbackTextEdit);

    ui->feedbackTextEditHolderLayout->addSpacing(10);
    _attachSystemInfoCheckboxButton = new CheckboxButton(tr("Attach information to hep us solve errors quicker."));
    _attachSystemInfoCheckboxButton->setChecked(true);
    _attachSystemInfoCheckboxButton->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
    _attachSystemInfoCheckboxButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    ui->feedbackTextEditHolderLayout->addWidget(_attachSystemInfoCheckboxButton);

    ui->feedbackTextEditHolderLayout->addSpacing(3);
    PlatformFlagsHelper::SummedSinglePlatformInformations *currSystemPlatformInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    QString currPlatformInfoPrintableText = "Undefined OS";
    if(currSystemPlatformInfo != NULL) {
        currPlatformInfoPrintableText = currSystemPlatformInfo->getAsPrintableText();
    }

    _attachedSystemInfoSummaryTextEdit = new QTextEdit;
    _attachedSystemInfoSummaryTextEdit->setHtml(tr("We will attach these informations:<br />- Information about your system: %1<br />").arg(currPlatformInfoPrintableText));
    _attachedSystemInfoSummaryTextEdit->setReadOnly(true);
    _attachedSystemInfoSummaryTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/Default/LightGray/style"));
    ui->feedbackTextEditHolderLayout->addWidget(_attachedSystemInfoSummaryTextEdit);

    _attachedSystemInfoSummaryTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    _attachedSystemInfoSummaryTextEditResizer = new WidgetAnimatedTwoSizeResizer(_attachedSystemInfoSummaryTextEdit, 500, QSize(0, 0), QSize(100, 100), true, false, true, WidgetAnimatedTwoSizeResizer::WRM_SetOnlyMaximumSize, this);
    connect(_attachSystemInfoCheckboxButton, SIGNAL(toggled(bool)), _attachedSystemInfoSummaryTextEditResizer, SLOT(switchState(bool)));


    ui->feedbackTextEditHolderLayout->addSpacing(10);
    ui->sendButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    ui->sendButton->setFlat(true);
    ui->sendButton->setText(tr("Send feedback"));

    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(_sendButtonPressed()));
}

void FeedbackPage::pagePushWillHappen()
{

}

void FeedbackPage::pageWillAppear(QString param)
{
    // activate the related menu item
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::feedbackPageMenuId());

    if(!param.isEmpty()) {
        if(param.startsWith(PAGE_OPTION_PREFIX_DEFAULTTEXT)) {
            _feedbackTextEdit->setText(param.mid(PAGE_OPTION_PREFIX_DEFAULTTEXT.length()));
        }
    }
}

void FeedbackPage::pageDidAppear()
{
//    qDebug("page did appear");
}

void FeedbackPage::pagePopWillHappen()
{
    // this is a root page, pop will never happen
}

void FeedbackPage::pageWillDisappear()
{

}

void FeedbackPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

void FeedbackPage::_sendButtonPressed() {
    DLog("Feedback content: ") << _feedbackTextEdit->toPlainText();

    if(_feedbackTextEdit->getNonPlaceholderText().isEmpty()) {
        DLog("Empty feedback, do nothing.");
        _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("You should write your feedback before sending it."));
        return;
    }

    disconnect(Authorization::sharedAuthorization());
    this->ui->sendButton->setDisabled(true);

    if (communicationDescriptionGateway) {
        disconnect(communicationDescriptionGateway);
        AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    }

    communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(communicationDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_authenticationChallengeHappened()));
    connect(communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_requestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_communicationError(QString)));

    QString feedbackText = _feedbackTextEdit->toPlainText();
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

void FeedbackPage::_authenticationChallengeHappened() {
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_sendButtonPressed()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginCommunicationError(QString)), this, SLOT(_communicationError(QString)));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(_requestFailed()));
}

void FeedbackPage::_requestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities) {
    this->ui->sendButton->setDisabled(false);
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Thank you for your feedback!"));
    _feedbackTextEdit->clear();

    // Todo
    // Show a 'Thank you' notification
}

void FeedbackPage::_communicationError(QString error) {
    this->_requestFailed();
}

void FeedbackPage::_requestFailed() {
    this->ui->sendButton->setDisabled(false);
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Failed to send it. We're truly ashamed, sorry..."));

    // Todo
    // Show a 'Failed' notification
}

void FeedbackPage::resizeEvent(QResizeEvent *e)
{
    BrowserPageWidgetBase::resizeEvent(e);

    _successFeedbackStatusPresenterWidget->setGeometryToThis(this->geometry());
}

QString FeedbackPage::getPageParameterForDefaultText(QString defaultFeedbackText)
{
    return QString("%1%2").arg(PAGE_OPTION_PREFIX_DEFAULTTEXT).arg(defaultFeedbackText);
}


