#include "suggestanapppage.h"
#include "ui_suggestanapppage.h"

#include "Application/Widgets/incontentnavigationwidget.h"
#include "Common/Widgets/contentcoverstatusmessagewidget.h"

// menu
#include "Common/Menu/menumanager.h"
#include "Settings/menuitemidcenter.h"
// global objects
#include "Application/globalobjectregistry.h"
#include "Common/Page/pagenavigationmanager.h"

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

// Network
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Common/Security/authorization.h"

SuggestAnAppPage::SuggestAnAppPage(QString pageId, QWidget *parent) :
    BrowserPageWidgetBase(pageId, parent),
    ui(new Ui::SuggestAnAppPageContentWidget),
    communicationDescriptionGateway(NULL)
{


//    ui->formLayout_2->setContentsMargins(0, 0, 0, 0);
//    this->setContentsMargins(15, 15, 15, 15);

    _successFeedbackStatusPresenterWidget = new ContentCoverStatusMessageWidget(300, 4000);
    _successFeedbackStatusPresenterWidget->setParent(this);
    _successFeedbackStatusPresenterWidget->hide();
}

SuggestAnAppPage::~SuggestAnAppPage()
{
    DLog("destr");
    delete ui;
}

void SuggestAnAppPage::initializePageContent()
{
    ui->setupUi(this);

    ui->mainLayout->setContentsMargins(0, 0, 0, 0);
    ui->mainLayout->setSpacing(3);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Options"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    ui->mainLayout->insertWidget(0, inContentNavigationWidget);

    ui->contentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    ui->appNameLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    ui->appNameLineEdit->setStyleSheet(guiConfManager->getStyleForId("LineEdit/Default/Gray/style"));
    ui->urlLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    ui->urlLineEdit->setStyleSheet(guiConfManager->getStyleForId("LineEdit/Default/Gray/style"));

    ui->summaryLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    ui->summaryTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/Default/LightGray/style"));
    ui->sendButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    ui->sendButton->setFlat(true);

    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(_sendButtonPressed()));
}

void SuggestAnAppPage::pagePushWillHappen()
{

}

void SuggestAnAppPage::pageWillAppear(QString param)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::suggestAnAppMenuId());

    DLog("param: ") << param;
    QString appNameParamPrefix("appName:");
    if(param.startsWith(appNameParamPrefix)) {
        ui->appNameLineEdit->setText(param.mid(appNameParamPrefix.length()));
    }
}

void SuggestAnAppPage::pageDidAppear()
{
//    qDebug("page did appear");
}

void SuggestAnAppPage::pagePopWillHappen()
{
}

void SuggestAnAppPage::pageWillDisappear()
{

}

void SuggestAnAppPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

void SuggestAnAppPage::_sendButtonPressed()
{
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

    communicationDescriptionGateway->requestForUserSuggestAnApp(ui->appNameLineEdit->text(),
                                                                ui->urlLineEdit->text(),
                                                                ui->summaryTextEdit->toPlainText());
}

void SuggestAnAppPage::_authenticationChallengeHappened() {
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_sendButtonPressed()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginCommunicationError(QString)), this, SLOT(_communicationError(QString)));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(_requestFailed()));
}

void SuggestAnAppPage::_requestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities) {
    this->ui->sendButton->setDisabled(false);
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Thank you, we'll try to include your beloved app in our collection."));
    this->ui->appNameLineEdit->clear();
    this->ui->urlLineEdit->clear();
    this->ui->summaryTextEdit->clear();

    // Todo
    // Show a 'Thank you' notification
}

void SuggestAnAppPage::_communicationError(QString error) {
    this->_requestFailed();
}

void SuggestAnAppPage::_requestFailed() {
    this->ui->sendButton->setDisabled(false);
    _successFeedbackStatusPresenterWidget->showWithAnimationAndActivate(tr("Failed to send it. We're truely ashamed, sorry..."));

    // Todo
    // Show a 'Failed' notification
}

void SuggestAnAppPage::resizeEvent(QResizeEvent *e)
{
    BrowserPageWidgetBase::resizeEvent(e);

    _successFeedbackStatusPresenterWidget->setGeometryToThis(this->geometry());
}
