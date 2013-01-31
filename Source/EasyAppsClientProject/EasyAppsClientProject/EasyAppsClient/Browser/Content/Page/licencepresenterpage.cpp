#include "licencepresenterpage.h"

// UI
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#include "Application/Widgets/incontentnavigationwidget.h"

// network
#include "Application/Communication/Entity/getappeulanetworkentity.h"
#include "Application/Communication/Request/getappeulanetworkrequest.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
#include "Common/Page/pagenavigationmanager.h"

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Settings/menuitemidcenter.h"
#include "Settings/pageidcenter.h"

LicencePresenterPage::LicencePresenterPage(QString pageId, QString relatedAppId, QWidget *parent) :
    PageWidgetBase(pageId, parent),
    _relatedAppId(relatedAppId),
    communicationDescriptionGateway(0)
{

}

void LicencePresenterPage::initializePageContent()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
//    this->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));
    this->setContentsMargins(0, 0, 0, 0);

    this->setStyleSheet(guiConfManager->getStyleForId("WidgetClassName(%1)/Default/Background/style").arg("LicencePresenterPage"));

//    QPushButton *backButton = new QPushButton;
//    backButton->setText(tr("Back"));
//    connect(backButton, SIGNAL(clicked()), this, SLOT(_backButtonPressed()));
//    backButton->setFixedSize(100, 30);
//    backButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
//    mainLayout->addWidget(backButton);

    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Back to the app page"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->addWidget(inContentNavigationWidget);

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    _licenceTextEdit = new QTextEdit;
    _licenceTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/NoBorder/LightGray/style"));
    _licenceTextEdit->setReadOnly(true);
    _licenceTextEdit->setPlainText("Loading...");

    contentLayout->addWidget(_licenceTextEdit);

    mainLayout->addLayout(contentLayout);
}

void LicencePresenterPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

void LicencePresenterPage::pagePushWillHappen()
{
    // Create menu
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();

    MenuItemWidget *applicationMenu = new MenuItemWidget(MenuItemIdCenter::licencePresenterPageMenuId(), "Licence");
    connect(applicationMenu, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(relatedMenuItemActivated(QString)));

    menuManager->addChildToItem(applicationMenu, MenuItemIdCenter::appDetailPageMenuItem());
}

void LicencePresenterPage::pageWillAppear(QString param)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::licencePresenterPageMenuId());
}

void LicencePresenterPage::pageDidAppear()
{
    // Load the licence from the given url

    AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(failedToGetDescription(QString)));

    communicationDescriptionGateway->performThisRequest(new GetAppEulaNetworkRequest(_relatedAppId));
}

void LicencePresenterPage::networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities)
{
    DLog("Licence downloaded.");

    if(entities.isEmpty()) {
        this->_licenceTextEdit->setPlainText(tr("No Licence found."));
        return;
    }

    GetAppEulaNetworkEntity *entity = dynamic_cast<GetAppEulaNetworkEntity *>(entities[0]);
    if(entity == NULL) {
        this->_licenceTextEdit->setPlainText(tr("Internal error. We're totally embarassed. Sorry."));
    }
    else {
        this->_licenceTextEdit->setPlainText(entity->getEulaText());
    }
}

void LicencePresenterPage::failedToGetDescription(QString errorMessage)
{
    this->_licenceTextEdit->setPlainText(tr("Cannot get the related Licence. Error: %1").arg(errorMessage));
}

void LicencePresenterPage::pagePopWillHappen()
{
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    menuManager->removeItemById(MenuItemIdCenter::licencePresenterPageMenuId());
}

void LicencePresenterPage::pageWillDisappear()
{

}


void LicencePresenterPage::relatedMenuItemActivated(QString menuItemId) {
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::storePageId())) {
        // this root page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(this->getPageId());
    } else {
        // !! it's not a root page, unnecessary
        // currently this root page's stack is not the active one, so switch to it
//        PageNavigationManager::sharedNavigationManager()->switchToRootPageStack(PageIdCenter::grabSomeAppsPageId());
    }
}
