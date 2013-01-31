#include "applicationstoreapplicationpage.h"

// UI
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QScrollArea>
#include <QSpacerItem>
#include <QNetworkProxyFactory>
#include <QWebSettings>
#include <QWebView>
#include <QWebFrame>
#include <QScrollBar>

#include "Browser/Content/Widget/plainerrorpresenterwidget.h"
#include "Common/multicontentmanager.h"
#include "Common/Widgets/urlbutton.h"
#include "Common/Widgets/popupwebviewwidget.h"
#include "Application/Widgets/incontentnavigationwidget.h"
#include "Common/Widgets/magnetwidget.h"

//
#include <QResizeEvent>
#include <QTimer>

// page navigation
#include "Common/Page/pagenavigationmanager.h"
#include "Browser/Content/Page/licencepresenterpage.h"
// menu
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
// global objects
#include "Application/globalobjectregistry.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"

#include "Browser/Content/Widget/ratingwidget.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

// ID
#include "Settings/pageidcenter.h"
#include "Settings/menuitemidcenter.h"

// Communication
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
//#include "Application/Communication/Gateway/communicationresourcegateway.h"
#include "Application/Communication/awresourcedownloadstreamtomemoryfactory.h"
#include "Common/Communication/basenetworkentity.h"
#include "Application/Communication/Entity/applicationnetworkentity.h"
#include "Application/Communication/Entity/applicationscreenshotentity.h"

// utility
#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"
#include "Application/Managers/AppGrab/grabapplicationmanager.h"
#include "Browser/Content/Widget/applicationthumbnailwidget.h"
#include "Common/Helpers/pathhelper.h"
#include "Common/Event/startthisapplicationevent.h"
#include <QDesktopServices>
#include "Application/Utils/awapplicationrunner.h"

#include "Common/Helpers/qstringhelper.h"
#include "Common/Helpers/qimagehelper.h"
#include "Common/Widgets/simpleimagepresenterwidget.h"

#include <QtWebKit/QWebView>

// config
#include "Settings/configmanager.h"
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

#define THUMB_SCROLL_AREA_FIXED_WIDTH 550

#define NORMAL_CONTENT_CONTENTID        QString("normal-content")
#define ERROR_CONTENT_CONTENTID        QString("error-content")

ApplicationStoreApplicationPage::ApplicationStoreApplicationPage(QString pageId, ApplicationDescriptionModel appDescriptionModel, QWidget *parent) :
    BrowserPageWidgetBase(pageId, parent),
//    applicationID(applicationID),
//    appName(appName),
    _appDescriptionModel(appDescriptionModel),
    _isContentLoaded(false)
{
//    this->setContentsMargins(0, 0, 0, 0);

    // Setup scroller
    mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
//    this->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    //
    // in-content navigation widget
    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Store"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->addWidget(inContentNavigationWidget);

    //
    //
    QVBoxLayout *pageContentLayout = new QVBoxLayout;
    mainLayout->addLayout(pageContentLayout);
    _pageContentManager = new MultiContentManager(pageContentLayout, this);

    //
    // error content
    PlainErrorPresenterWidget *errorContentWidget = new PlainErrorPresenterWidget;
    _pageContentManager->setContentWidgetToId(ERROR_CONTENT_CONTENTID, errorContentWidget);

    //
    // normal content
    QWidget *mainContentHolderWidget = new QWidget;
    QVBoxLayout* mainContentHolderLayout = new QVBoxLayout();
    mainContentHolderWidget->setLayout(mainContentHolderLayout);
    mainContentHolderWidget->setContentsMargins(0, 0, 0, 0);
    mainContentHolderLayout->setContentsMargins(0, 0, 0, 0);
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("ScrollBar/Default/GrayGreen/style"));
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidgetResizable(true);
    QWidget* scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setContentsMargins(0, 0, 0, 0);

    appDetailContentHolderLayout = new QHBoxLayout;
    scrollAreaWidgetContents->setLayout(appDetailContentHolderLayout);
    appDetailContentHolderLayout->setContentsMargins(0, 0, 0, 0);
    appDetailContentHolderLayout->setSpacing(0);

    scrollArea->setWidget(scrollAreaWidgetContents);
    mainContentHolderLayout->addWidget(scrollArea);
    scrollArea->setFrameShape(QFrame::NoFrame);

    _pageContentManager->setContentWidgetToId(NORMAL_CONTENT_CONTENTID, mainContentHolderWidget);

    this->setLayout(mainLayout);


    QWidget* contentWidget = new QWidget;
    contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentWidget->setLayout(contentLayout);

    contentWidget->setObjectName("StoreAppdetailsContentwidget");
    contentWidget->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style/ObjectName(%1)").arg("StoreAppdetailsContentwidget"));


    contentWidget->setContentsMargins(0, 0, 0, 0);

    QWidget* sidebarWidget = new QWidget;
    sidebarWidget->setObjectName("sidebarWidget");
    sidebarWidget->setStyleSheet(guiConfManager->getStyleForId("Store/AppDetails/ManagementSidebar/style"));
    _sidebarLayout = new QVBoxLayout;
    sidebarWidget->setLayout(_sidebarLayout);

    sidebarWidget->setFixedWidth(215);
    sidebarWidget->setContentsMargins(0, 0, 0, 0);
    _sidebarLayout->setContentsMargins(10, 10, 10, 10);
    _sidebarLayout->setSpacing(3);
    sidebarWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    appDetailContentHolderLayout->addWidget(contentWidget);
    appDetailContentHolderLayout->addWidget(sidebarWidget);
}

void ApplicationStoreApplicationPage::initializePageContent() {

    // ---
    // content widget creation

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

//    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));
    this->setStyleSheet(guiConfManager->getStyleForId("WidgetClassName(%1)/Default/Background/style").arg("ApplicationStoreApplicationPage"));


    //
    // title
    QWidget *titleContainerWidget = new QWidget;
    this->titleLayout = new QHBoxLayout;
    titleContainerWidget->setLayout(titleLayout);
    titleContainerWidget->setContentsMargins(0, 0, 0, 0);

    // Title    
    this->title = new QLabel(this->_getCurrAppName());
    this->title->setStyleSheet("* { font: 18pt Arial sans-serif; color: #474747; }");

    // Rating
//    this->ratingWidget = new RatingWidget;

    // Description
    this->description = new QLabel;
    this->description->setStyleSheet("* { font: 12pt Arial sans-serif; color: #676767; }");
    this->description->setWordWrap(true);

    // More to see
    this->moreLabel = new QLabel;
    this->moreLabel->setStyleSheet("* { font: 12pt Arial sans-serif; color: #676767; font-weight: bold; }");

    QWidget *relatedMediaBackgroundWidget = new QWidget;
    relatedMediaBackgroundWidget->setContentsMargins(0, 0, 0, 0);
    relatedMediaBackgroundWidget->setObjectName("thumbnailBackgroundWidget");
    relatedMediaBackgroundWidget->setStyleSheet(guiConfManager->getStyleForId("Store/AppDetails/RelatedMediaContainer/style"));
    relatedMediaBackgroundWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout *relatedMediaBackgroundLayout = new QVBoxLayout;
    relatedMediaBackgroundWidget->setLayout(relatedMediaBackgroundLayout);

    // Full sized video
    // Todo: valtasnal villodzik... kepeket is innen tolthetnenk be?!
//    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    _relatedFullSizeMediaContainerLayout = new QVBoxLayout;


    // Full sized image
    SimpleImagePresenterWidget *fullSizedImage = new SimpleImagePresenterWidget;
    fullSizedImage->setIsAutoResizeToImageSize(false);
    fullSizedImage->setFixedSize(550,344);
    _currFullSizeMediaPresenterWidget = fullSizedImage;
    _relatedFullSizeMediaContainerLayout->addWidget(fullSizedImage, 0 , Qt::AlignCenter);

    // Thumbnail container
    thumbnailLayout = new QHBoxLayout;
    thumbnailLayout->setSpacing(5);
    thumbnailLayout->setContentsMargins(0, 0, 0, 0);


    thumbnailScrollArea = new QScrollArea;
    thumbnailScrollArea->setWidgetResizable(false);
//    thumbnailScrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    thumbnailScrollArea->setFixedWidth(THUMB_SCROLL_AREA_FIXED_WIDTH);
//    thumbnailScrollArea->setBackgroundRole(QPalette::Dark);
    thumbnailScrollArea->setStyleSheet(guiConfManager->getStyleForId("ScrollBar/Default/GrayGreen/style"));
//    thumbnailScrollArea->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    thumbnailContainer = new QWidget;
    thumbnailContainer->setLayout(thumbnailLayout);
//    thumbnailLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    thumbnailScrollArea->setWidget(thumbnailContainer);

    relatedMediaBackgroundLayout->setSpacing(0);
//    relatedMediaBackgroundLayout->addWidget(fullSizedVideo, 0, Qt::AlignHCenter);
//    relatedMediaBackgroundLayout->addWidget(fullSizedImage, 0, Qt::AlignHCenter);
    relatedMediaBackgroundLayout->addLayout(_relatedFullSizeMediaContainerLayout);

//    QHBoxLayout *thumbBgLayout = new QHBoxLayout;
//    thumbBgLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
//    thumbBgLayout->addWidget(thumbnailScrollArea);
//    thumbBgLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    relatedMediaBackgroundLayout->addWidget(thumbnailScrollArea, 0, Qt::AlignHCenter);
//    relatedMediaBackgroundLayout->addLayout(thumbBgLayout);
//    contentLayout->addWidget(thumbnailBackgroundLayout);


    //
    // sidebar

    // Logo / icon
    this->_appIconImagePresenter = new SimpleImagePresenterWidget;
    this->_appIconImagePresenter->setFixedSize(200, 200);

    // Buttons
    this->_mainAppStateDependentButton = new QPushButton(tr("Grab"));
    this->_mainAppStateDependentButton->setEnabled(false);
    _mainAppStateDependentButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/HighPriority/style"));
    _mainAppStateDependentButton->setFlat(true);
    _mainAppStateDependentButton->setFixedHeight(30);

    QPushButton *showLicenceButton = new QPushButton(tr("Licence"));
    showLicenceButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    showLicenceButton->setFixedHeight(30);
    showLicenceButton->setFlat(true);
    connect(showLicenceButton, SIGNAL(clicked()), this, SLOT(_showLicenceButtonPressed()));

    // Dev website link
    _devWebsiteLinkLabel = new QLabel(tr("Developer's website"));
    connect(_devWebsiteLinkLabel, SIGNAL(linkActivated(QString)), this, SLOT(_openExternalLinkUrl(QString)));
    // support link
    _supportLinkLabel = new QLabel(tr("Support website"));
    connect(_supportLinkLabel, SIGNAL(linkActivated(QString)), this, SLOT(_openExternalLinkUrl(QString)));

    // app info
    _appInfoTitleLabel = new QLabel(tr("App Information"));
    _appInfoTitleLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Large/DarkGray/style"));
    _appInfoTitleLabel->setAlignment(Qt::AlignHCenter);
    // category
    QHBoxLayout *appInfoCategoryLayout = new QHBoxLayout;
//    QLabel *appInfoCategoryTextLabel = new QLabel(tr("Category:"));
//    appInfoCategoryTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    _appInfoCategoryLabel = new QLabel(tr("<span style=\"color:#333;\">Category:</span>"));
    _appInfoCategoryLabel->setStyleSheet(guiConfManager->getStyleForId("Label/SideBar/LightGray/style"));
//    appInfoCategoryLayout->addWidget(appInfoCategoryTextLabel);
    appInfoCategoryLayout->addWidget(_appInfoCategoryLabel);
    // version
    QHBoxLayout *appInfoVersionLayout = new QHBoxLayout;
//    _appInfoVersionTextLabel = new QLabel(tr("Version:"));
//    _appInfoVersionTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    _appInfoVersionLabel = new QLabel(tr("<span style=\"color:#333;\">Version:</span>"));
    _appInfoVersionLabel->setStyleSheet(guiConfManager->getStyleForId("Label/SideBar/LightGray/style"));
//    appInfoVersionLayout->addWidget(_appInfoVersionTextLabel);
    appInfoVersionLayout->addWidget(_appInfoVersionLabel);
    // price
    QHBoxLayout *appInfoPriceLayout = new QHBoxLayout;
//    QLabel *appInfoPriceTextLabel = new QLabel(tr("Price:"));
//    appInfoPriceTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    _appInfoPriceLabel = new QLabel(tr("<span style=\"color:#333;\">Price:</span>"));
    _appInfoPriceLabel->setStyleSheet(guiConfManager->getStyleForId("Label/SideBar/LightGray/style"));
//    appInfoPriceLayout->addWidget(appInfoPriceTextLabel);
    appInfoPriceLayout->addWidget(_appInfoPriceLabel);

    // requirements
    _appRequirementsTitleLabel = new QLabel(tr("Requirements"));
    _appRequirementsTitleLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Large/DarkGray/style"));
    _appRequirementsTitleLabel->setAlignment(Qt::AlignHCenter);
    // windows
    QHBoxLayout *appRequirementsWindowsLayout = new QHBoxLayout;
//    QLabel *appRequirementsWindowsTextLabel = new QLabel(tr("Windows:"));
//    appRequirementsWindowsTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    _appRequirementsWindowsLabel = new QLabel(tr("<span style=\"color:#333;\">Windows:</span>"));
    _appRequirementsWindowsLabel->setStyleSheet(guiConfManager->getStyleForId("Label/SideBar/LightGray/style"));
    _appRequirementsWindowsLabel->setWordWrap(true);
//    appRequirementsWindowsLayout->addWidget(appRequirementsWindowsTextLabel);
    appRequirementsWindowsLayout->addWidget(_appRequirementsWindowsLabel);
    // mac
    QHBoxLayout *appRequirementsMacLayout = new QHBoxLayout;
//    QLabel *appRequirementsMacTextLabel = new QLabel(tr("Mac:"));
//    appRequirementsMacTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    _appRequirementsMacLabel = new QLabel(tr("<span style=\"color:#333;\">Mac:</span>"));
    _appRequirementsMacLabel->setStyleSheet(guiConfManager->getStyleForId("Label/SideBar/LightGray/style"));
    _appRequirementsMacLabel->setWordWrap(true);
//    appRequirementsMacLayout->addWidget(appRequirementsMacTextLabel);
    appRequirementsMacLayout->addWidget(_appRequirementsMacLabel);
    // hard drive
    QHBoxLayout *appRequirementsHddLayout = new QHBoxLayout;
//    _appRequirementsHddTextLabel = new QLabel(tr("Hdd:"));
//    _appRequirementsHddTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    _appRequirementsHddLabel = new QLabel(tr("<span style=\"color:#333;\">Hdd:</span>"));
    _appRequirementsHddLabel->setStyleSheet(guiConfManager->getStyleForId("Label/SideBar/LightGray/style"));
//    appRequirementsHddLayout->addWidget(_appRequirementsHddTextLabel);
    appRequirementsHddLayout->addWidget(_appRequirementsHddLabel);


    // Twitter
#if 0
    this->twitterView = new QWebView;
    this->twitterView->setContentsMargins(0, 0, 0, 0);

    QFile res(":/web/twitter");
    res.open(QIODevice::ReadOnly|QIODevice::Text);
    this->twitterView->setHtml(res.readAll());

//    QUrl twitterUrl = QUrl(QString(":/web/twitter"));
//    this->twitterView->load(QUrl("qrc:/web/twitter"));
//    this->twitterView->load(QUrl("qrc:/web/twitter?query=#microsoft OR #apple"));
#endif

    // Connections
    connect(_mainAppStateDependentButton, SIGNAL(clicked()), this, SLOT(mainAppStateDependentButtonClicked()));
    connect(moreLabel, SIGNAL(linkActivated(QString)), this, SLOT(morePressed(QString)));


    // ---------
    // Set the layout

    //
    // main content layout

    this->titleLayout->addWidget(this->title);
    this->titleLayout->addStretch();
//    this->titleLayout->addWidget(this->ratingWidget);

//    this->contentLayout->addLayout(headerLayout);
//    this->contentLayout->addLayout(this->titleLayout);
//    this->contentLayout->addWidget(headerContainerWidget);
    this->contentLayout->addWidget(titleContainerWidget);
    this->contentLayout->addSpacing(10);
    this->contentLayout->addWidget(this->description);
    this->contentLayout->addWidget(moreLabel);
    this->contentLayout->addSpacing(10);
//    this->contentLayout->addWidget(fullSizedVideo, 0, Qt::AlignHCenter);
//    this->contentLayout->addWidget(fullSizedImage, 0, Qt::AlignHCenter);
//    this->contentLayout->addWidget(thumbnailScrollArea, 0, Qt::AlignHCenter);
    contentLayout->addWidget(relatedMediaBackgroundWidget);
    this->contentLayout->insertStretch(-1, 1);


    //
    // sidebar layout

    _sidebarLayout->addWidget(this->_appIconImagePresenter);
    _sidebarLayout->addWidget(this->_mainAppStateDependentButton);
    _sidebarLayout->addWidget(showLicenceButton);
    _sidebarLayout->addSpacerItem(new QSpacerItem(0, 10));
    // links
    _sidebarLayout->addWidget(_devWebsiteLinkLabel);
    _sidebarLayout->addWidget(_supportLinkLabel);
    _sidebarLayout->addSpacerItem(new QSpacerItem(0, 10));
    // app info
    _sidebarLayout->addWidget(_appInfoTitleLabel);
    _sidebarLayout->addLayout(appInfoCategoryLayout);
    _sidebarLayout->addLayout(appInfoVersionLayout);
    _sidebarLayout->addLayout(appInfoPriceLayout);
    // requirements
    _sidebarLayout->addSpacerItem(new QSpacerItem(0, 10));
    _sidebarLayout->addWidget(_appRequirementsTitleLabel);
    _sidebarLayout->addLayout(appRequirementsWindowsLayout);
    _sidebarLayout->addLayout(appRequirementsMacLayout);
    _sidebarLayout->addLayout(appRequirementsHddLayout);
//    this->sidebarLayout->addWidget(this->twitterView);
    this->_sidebarLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
//    this->sidebarLayout->insertStretch(-1, 1);

//    fullSizedVideo->hide();
//    fullSizedImage->hide();

    _pageContentManager->switchContent(NORMAL_CONTENT_CONTENTID);

    this->update();
    this->updateGeometry();
}

void ApplicationStoreApplicationPage::_appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    if(this->_getCurrAppId() == appId) {
        this->refreshByRelatedAppState(newAppState);
    }
}

//void ApplicationStoreApplicationPage::_getAppStateResult(QString appId, AppWhirr::ApplicationStates::ApplicationStatesEnum newAppState, QString operationId, bool isQuerySuccess)
//{
//    if(operationId != "ApplicationStoreApplicationPage-AppStateRefresh")
//    {
//        // this is not our operation, skip it
//        return;
//    }

//    if(appId == this->_getCurrAppId()) {
//        if(!isQuerySuccess) {
//            // probably not found - not yet grabbed or installed
//            this->refreshByRelatedAppState(AppWhirr::ApplicationStates::NoState);
//        }
//        else {
//            this->refreshByRelatedAppState(newAppState);
//        }
//    }
//}

void ApplicationStoreApplicationPage::_getAppDescriptionResult(ApplicationDescriptionModel resultAppDescription, QString dbOperationId, bool isQuerySuccess)
{
    if(dbOperationId != "ApplicationStoreApplicationPage-AppStateRefresh")
    {
        // this is not our operation, skip it
        return;
    }

    if(resultAppDescription.getDynamicDescription_ReadOnly()->getAppId() == this->_getCurrAppId()) {
        // disconnect - not interested anymore -> will be informed about changes
        disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel,QString,bool)), this, SLOT(_getAppDescriptionResult(ApplicationDescriptionModel,QString,bool)));

        if(!isQuerySuccess) {
            // probably not found - not yet grabbed or installed
            this->refreshByRelatedAppState(AppWhirr::ApplicationGrabStates::NoState);
        }
        else {
            this->refreshByRelatedAppDescription(resultAppDescription);
        }
    }
}

void ApplicationStoreApplicationPage::_batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdAndNewStateMap)
{
    if(appIdAndNewStateMap.contains(this->_getCurrAppId())) {
        this->refreshByRelatedAppState(appIdAndNewStateMap.value(this->_getCurrAppId()));
    }
}

void ApplicationStoreApplicationPage::_wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel)
{
    if(this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId() == updatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId()) {
        this->refreshByRelatedAppDescription(updatedAppDescriptionModel);
    }
}

void ApplicationStoreApplicationPage::refreshByRelatedAppDescription(ApplicationDescriptionModel newAppDescriptionModel)
{
    DLog(" --- Versions: ") << newAppDescriptionModel.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform() << newAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledVersion();
    DLog(" -- Additional: ") << newAppDescriptionModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags();
    this->_appDescriptionModel = newAppDescriptionModel;
    this->refreshByRelatedAppState(this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppState());
}

void ApplicationStoreApplicationPage::refreshByRelatedAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    _appDescriptionModel.getDynamicDescription_DataForModification()->setAppState(newAppState);

    if(!_appDescriptionModel.isCompatibleWithTheCurrentPlatform()) {
        _mainAppStateDependentButton->setEnabled(false);
        _mainAppStateDependentButton->setText(tr("Show alternative"));
    }

    else {
        if(newAppState == AppWhirr::ApplicationGrabStates::Installed) {
            _mainAppStateDependentButton->setEnabled(true);
            if(_appDescriptionModel.isUpdateAvailable()) {
                _mainAppStateDependentButton->setText(tr("Update"));
            }
            else {
                _mainAppStateDependentButton->setText(tr("Run"));
            }
        }

        else if(newAppState == AppWhirr::ApplicationGrabStates::InstalledAndUpdating) {
            _mainAppStateDependentButton->setEnabled(false);
            _mainAppStateDependentButton->setText(tr("Updating..."));
        }

        else if(newAppState == AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled) {
            _mainAppStateDependentButton->setEnabled(true);
            _mainAppStateDependentButton->setText(tr("Install"));
        }

        else if(newAppState == AppWhirr::ApplicationGrabStates::GrabbedAndInstalling) {
            _mainAppStateDependentButton->setEnabled(false);
            _mainAppStateDependentButton->setText(tr("Installing..."));
        }

        else if(newAppState == AppWhirr::ApplicationGrabStates::GrabbedAndUninstalling) {
            _mainAppStateDependentButton->setEnabled(false);
            _mainAppStateDependentButton->setText(tr("Uninstalling..."));
        }

        else {
            _mainAppStateDependentButton->setEnabled(true);
            _mainAppStateDependentButton->setText(tr("Grab and Install"));
        }
    }
}

void ApplicationStoreApplicationPage::pagePushWillHappen()
{
    // Create menu
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();

    MenuItemWidget *applicationMenu = new MenuItemWidget(MenuItemIdCenter::appDetailPageMenuItem(), this->_getCurrAppName());
    connect(applicationMenu, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(relatedMenuItemActivated(QString)));

    menuManager->addChildToItem(applicationMenu, MenuItemIdCenter::storeMenuId());
}

// Page will be navigated, let's start the communication
void ApplicationStoreApplicationPage::pageWillAppear(QString param)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::appDetailPageMenuItem());
}

void ApplicationStoreApplicationPage::pageDidAppear() {

    if(_isContentLoaded) {
        // already loaded, don't load it again

        if(!_lastSelectedThumbnailInfo.getUrl().isEmpty()) {
            // reload the last selected thumbnail
            this->thumbnailSelected(_lastSelectedThumbnailInfo.getUrl(), _lastSelectedThumbnailInfo.getThumbnailPixmap(), _lastSelectedThumbnailInfo.getIsImage());
        }
        return;
    }


    // get page content
    communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(failedToGetDescription(QString)));

    communicationDescriptionGateway->requestForApplication(this->_getCurrAppId());

    LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();

    connect(localAppDatabaseManager, SIGNAL(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)), this, SLOT(_appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)));
//    connect(localAppDatabaseManager, SIGNAL(_result_getAppStateByAppId(QString,AppWhirr::ApplicationStates::ApplicationStatesEnum,QString,bool)), this, SLOT(_getAppStateResult(QString,AppWhirr::ApplicationStates::ApplicationStatesEnum,QString,bool)));
    connect(localAppDatabaseManager, SIGNAL(batchAppStateChanged(QMap<QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum>)), this, SLOT(_batchAppStateChanged(QMap<QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum>)));

    connect(localAppDatabaseManager, SIGNAL(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)), this, SLOT(_wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)));

    // and refresh the state
    connect(localAppDatabaseManager, SIGNAL(_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel,QString,bool)), this, SLOT(_getAppDescriptionResult(ApplicationDescriptionModel,QString,bool)));
//    localAppDatabaseManager->getAppStateByAppId(this->_getCurrAppId(), "ApplicationStoreApplicationPage-AppStateRefresh");
    localAppDatabaseManager->getAppDescriptionModelByAppId(this->_getCurrAppId(), "ApplicationStoreApplicationPage-AppStateRefresh");


    _isContentLoaded = true;
}

void ApplicationStoreApplicationPage::pagePopWillHappen() {
    DLog("Page wil be popped");
//    DLog("--- pop, remove menu");
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    menuManager->removeItemById(MenuItemIdCenter::appDetailPageMenuItem());
}

void ApplicationStoreApplicationPage::pageWillDisappear() {
//    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(appStateChanged(QString)), this, SLOT(_appStateChanged(QString)));
//    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(batchAppStateChanges(QStringList)), this, SLOT(_batchAppStateChanged(QStringList)));

    // remove the current screenshot / video
    _relatedFullSizeMediaContainerLayout->removeWidget(_currFullSizeMediaPresenterWidget);
    _currFullSizeMediaPresenterWidget->deleteLater();
    _currFullSizeMediaPresenterWidget = NULL;
}

// Parse the response description
void ApplicationStoreApplicationPage::networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("networkRequestFinished");
#endif

    AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    _resourceDownloadStreamToMemory = AW::Client::Communication::AWResourceDownloadStreamToMemoryFactory::createDefaultResourceDownloadStreamToMemory(NULL);

    AW::Common::Communication::BaseNetworkEntity* entity = NULL;
    Q_FOREACH(entity, entities) {
        appEntity = dynamic_cast<ApplicationDetailInfosNetworkEntity *>( entity );
        if(appEntity == NULL) {
            WLog("Invalid cast.");
            continue;
        }

        MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
        MenuItemWidget *applicationMenu = menuManager->getItemById(MenuItemIdCenter::appDetailPageMenuItem());
        if(applicationMenu == NULL) {
            WLog("Cannot find (or cast) the related menu item");
        }
        else {
            DLog("Menu item found, app name: ") << appEntity->getApplicationName();
            applicationMenu->setTitle(appEntity->getApplicationName());
        }

        this->title->setText(appEntity->getApplicationName());
//        this->ratingWidget->updateRating(appEntity->getApplicationRating());
        fullDescription = appEntity->getApplicationDescription();
        this->description->setText(QStringHelper::truncate(fullDescription, 300));

        if (fullDescription.length() > 300) {
            this->moreLabel->setText("<a href style=\"text-decoration:none; color:#575757;\">Find out more...</a>");
        } else {
            this->contentLayout->removeWidget(this->moreLabel);
            delete this->moreLabel, moreLabel = NULL;
        }

//        _appDescriptionModel.getDynamicDescription_DataForModification()->setSupportedPlatformFlags(appEntity->get);

        // links
        _devWebsiteLinkLabel->setText(QString("<a href='%1' style=\"text-decoration:none; color:#575757;\">%2</a>").arg(appEntity->getApplicationDeveloperURL()).arg(tr("Developer's website")));
        _supportLinkLabel->setText(QString("<a href='%1' style=\"text-decoration:none; color:#575757;\">%2</a>").arg(appEntity->getSupportUrl()).arg(tr("Support website")));

        // app info
//        if(appEntity->getApplicationCategory().isEmpty()) {
//            _appInfoCategoryLabel->setText(tr("not found"));
//        }
//        else {
        _appInfoCategoryLabel->setText(_appInfoCategoryLabel->text() + " " + appEntity->getApplicationCategory());
//        }
        if(appEntity->getApplicationVersion().isEmpty()) {
            _appInfoVersionLabel->hide();
//            _appInfoVersionTextLabel->hide();
        }
        else {
            _appInfoVersionLabel->setText(_appInfoVersionLabel->text() + " " + appEntity->getApplicationVersion());
        }
//        if(appEntity->getPrice().isEmpty()) {
//            _appInfoPriceLabel->setText(tr("not found"));
//        }
//        else {
            _appInfoPriceLabel->setText(_appInfoPriceLabel->text() + " " + appEntity->getPrice());
//        }

        // requirements
        QString winAppend = !appEntity->getApplicationWindowsReadableFlags().isEmpty()
                ? appEntity->getApplicationWindowsReadableFlags()
                : tr("Not supported");
        QString macAppend = !appEntity->getApplicationMacReadableFlags().isEmpty()
                ? appEntity->getApplicationMacReadableFlags()
                : tr("Not supported");
        _appRequirementsWindowsLabel->setText(_appRequirementsWindowsLabel->text() + " " + winAppend);
        _appRequirementsMacLabel->setText(_appRequirementsMacLabel->text() + " " + macAppend);
        if(appEntity->getApplicationSize() <= 0) {
            _appRequirementsHddLabel->hide();
//            _appRequirementsHddTextLabel->hide();
        }
        else {
            _appRequirementsHddLabel->setText(_appRequirementsHddLabel->text() + " " + QString("%1 MB").arg(QString::number(appEntity->getApplicationSize()/1048576.0, 10, 2)));
        }

        // media + thumbnail
        QString url;
        int biggestThumbHeight = -1;
        int thumbnailContainerWidth = 0;

        // TODO: Kovi updatere Mac-re visszarakni es rajonni, mi a hiba
#ifdef Q_OS_WIN
        Q_FOREACH(url, appEntity->getApplicationVideos()) {
            QString youtubeImageURL = "http://img.youtube.com/vi/" + url + "/1.jpg";
            ApplicationThumbnailWidget* thumbnail = new ApplicationThumbnailWidget(youtubeImageURL, false);
            connect(thumbnail, SIGNAL(thumbnailSelected(QString,QPixmap,bool)), this, SLOT(thumbnailSelected(QString,QPixmap,bool)));
            thumbnail->addImageLayer(LayeredImagePresenterWidget::ImageLayerModel(QImageHelper::createImageByPath(QString("://images/store/VideoThumbnail-IndicatorCover"), ImageTypes::PixelBased), LayeredImagePresenterWidget::ImageLayerAlignment::Center, LayeredImagePresenterWidget::ImageLayerResizeMode::NoResize));

            if (!thumbnailLayout->count()) {
                thumbnail->setSelectedWhenResourceIsReady(true);
            }
            thumbnailLayout->addWidget(thumbnail);
            thumbnailContainerWidth += thumbnail->sizeHint().width() + thumbnailLayout->spacing();
            if (thumbnail->sizeHint().height() > biggestThumbHeight) {
                biggestThumbHeight = thumbnail->sizeHint().height();
            }

            _resourceDownloadStreamToMemory->addResourceDownloadRequest(youtubeImageURL);
            connect(_resourceDownloadStreamToMemory, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), thumbnail, SLOT(retrievedResourceForURL(QUrl,QByteArray)));
        }
#endif
        ApplicationScreenshotEntity* screenshotEntity;
        Q_FOREACH(screenshotEntity,  appEntity->getApplicationImages()) {
            ApplicationThumbnailWidget* thumbnail = new ApplicationThumbnailWidget(screenshotEntity->getThumbnailUrl(), true);
            connect(thumbnail, SIGNAL(thumbnailSelected(QString,QPixmap,bool)), this, SLOT(thumbnailSelected(QString,QPixmap,bool)));

            if (!thumbnailLayout->count()) {
                thumbnail->setSelectedWhenResourceIsReady(true);
            }
            thumbnailLayout->addWidget(thumbnail);
//            thumbnailContainer->setFixedWidth(thumbnailContainer->width() + thumbnail->sizeHint().width() + thumbnailLayout->spacing());
            thumbnailContainerWidth += thumbnail->sizeHint().width() + thumbnailLayout->spacing();
            if (thumbnail->sizeHint().height() > biggestThumbHeight) {
//                thumbnailContainer->setFixedHeight(thumbnail->sizeHint().height());
                biggestThumbHeight = thumbnail->sizeHint().height();
            }

            _resourceDownloadStreamToMemory->addResourceDownloadRequest(screenshotEntity->getThumbnailUrl());
            connect(_resourceDownloadStreamToMemory, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), thumbnail, SLOT(retrievedResourceForURL(QUrl,QByteArray)));
        }

        if(biggestThumbHeight > 0) {
            DLog("Biggest thumb height: ") << biggestThumbHeight;
//            DLog("Size hint: ") << thumbnail->sizeHint();
            thumbnailContainer->setFixedHeight(biggestThumbHeight);
            if(thumbnailContainerWidth < THUMB_SCROLL_AREA_FIXED_WIDTH) {
                thumbnailScrollArea->setFixedHeight(biggestThumbHeight + 2); // +2 = treshold for removing vertical scrollbars
            } else {
                thumbnailScrollArea->setFixedHeight(biggestThumbHeight + thumbnailScrollArea->horizontalScrollBar()->height() + 2); // +2 = treshold for removing vertical scrollbars
            }
        }
        else {
            WLog("Invalid thumbnail biggest height.");
        }
        if(thumbnailContainerWidth > 0) {
            thumbnailContainer->setFixedWidth(thumbnailContainerWidth);
        }
        else {
            WLog("Invalid thumb-container width");
        }

//        thumbnailLayout->insertStretch(-1, 1);

        // Add image URL to the list
        _resourceDownloadStreamToMemory->addResourceDownloadRequest(appEntity->getApplicationIconURL());
        connect(_resourceDownloadStreamToMemory, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), this, SLOT(retrievedResourceForURL(QUrl,QByteArray)));
    }
    connect(_resourceDownloadStreamToMemory, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(networkResourceRequestsFinished()));

    // Start loading all requests
    _resourceDownloadStreamToMemory->startDownloadingResources();
}

void ApplicationStoreApplicationPage::failedToGetDescription(const QString& error) {
    DLog(error);

    PlainErrorPresenterWidget *errorContentWidget = dynamic_cast<PlainErrorPresenterWidget *>( _pageContentManager->getContentWidget(ERROR_CONTENT_CONTENTID) );
    if(errorContentWidget == NULL) {
        WLog("Invalid error-content widget. Cannot cast it.");
    }
    else {
        errorContentWidget->setErrorMessageByDefaultMessageEnum(PlainErrorPresenterWidget::DEM_ServerUnavailable);
        _pageContentManager->switchContent(ERROR_CONTENT_CONTENTID);
    }
}

void ApplicationStoreApplicationPage::networkResourceRequestsFinished() {
    AW_QOBJECT_SAFE_DELETE(_resourceDownloadStreamToMemory);
}

void ApplicationStoreApplicationPage::retrievedResourceForURL(QUrl URL, QByteArray resource) {
    if (appEntity->getApplicationIconURL() != URL.toString()) return;

    _appIconImagePresenter->setImageByByteArray(resource, ImageTypes::PixelBased);
}

QList<QString> ApplicationStoreApplicationPage::getSubpageRequestURLs() {
    return QList<QString>();
}

void ApplicationStoreApplicationPage::relatedMenuItemActivated(QString menuItemId)
{
    DLog("-- Menu item activated") << menuItemId << this->getPageId();
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::storePageId())) {
        // this page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(this->getPageId());
    } else {
        // !! it's not a root page, unnecessary
        // currently this root page's stack is not the active one, so switch to it
//        PageNavigationManager::sharedNavigationManager()->switchToRootPageStack(PageIdCenter::grabSomeAppsPageId());
    }
}

void ApplicationStoreApplicationPage::mainAppStateDependentButtonClicked()
{

    if(this->_getCurrAppState() == AppWhirr::ApplicationGrabStates::Installed) {
        if(this->_appDescriptionModel.isUpdateAvailable()) {
            // update it
            this->refreshByRelatedAppState(AppWhirr::ApplicationGrabStates::InstalledAndUpdating);
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->grabApplication(this->_getCurrAppId(), this->_getCurrAppName(), this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), true, this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
        }
        else {
            // run it
            this->_runCurrentApp();
        }
    }

    else {
        // grab it
        this->refreshByRelatedAppState(AppWhirr::ApplicationGrabStates::GrabbedAndInstalling);
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->grabApplication(this->_getCurrAppId(), this->_getCurrAppName(), this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), false, this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
    }
}

void ApplicationStoreApplicationPage::_runCurrentApp()
{
    AW::Client::Utils::AWApplicationRunner::postRunThisAppEvent(_appDescriptionModel);
}

void ApplicationStoreApplicationPage::morePressed(QString url)
{
    this->description->setText(this->fullDescription);
    this->contentLayout->removeWidget(this->moreLabel);
    delete this->moreLabel, this->moreLabel = NULL;
}

QString ApplicationStoreApplicationPage::_getCurrAppName()
{
    return this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppName();
}

QString ApplicationStoreApplicationPage::_getCurrAppId()
{
    return this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId();
}

AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum ApplicationStoreApplicationPage::_getCurrAppState()
{
    return this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppState();
}

void ApplicationStoreApplicationPage::thumbnailSelected(QString url, QPixmap thumbnailPixmap, bool image)
{
    DLog("Thumbnail selected with url: ") << url;
    _lastSelectedThumbnailInfo = ApplicationStoreApplicationPage::ThumbnailItemModel(url, thumbnailPixmap, image);

    if (image) {
        // image

        SimpleImagePresenterWidget *fullSizedImage = dynamic_cast<SimpleImagePresenterWidget *>( _currFullSizeMediaPresenterWidget );
        if (fullSizedImage == NULL) {
            // remove the current widget, which is a web-view and replace it with an image presenter widget
            _relatedFullSizeMediaContainerLayout->removeWidget(_currFullSizeMediaPresenterWidget);
            _currFullSizeMediaPresenterWidget->deleteLater();
            fullSizedImage = new SimpleImagePresenterWidget;
            fullSizedImage->setIsAutoResizeToImageSize(false);
            fullSizedImage->setFixedSize(550,344);

            _currFullSizeMediaPresenterWidget = fullSizedImage;
            _relatedFullSizeMediaContainerLayout->addWidget(_currFullSizeMediaPresenterWidget, 0, Qt::AlignCenter);
//            fullSizedImage->show();
        }

//            fullSizedVideo->setHtml("");
//            fullSizedVideo->hide();
 //       }
        fullSizedImage->setImageByPixmap(thumbnailPixmap);

        ApplicationScreenshotEntity* screenshotEntity;
        Q_FOREACH(screenshotEntity, appEntity->getApplicationImages()) {
            if (screenshotEntity->getThumbnailUrl() == url) {
                // Check if we have already downloaded it
                QString URL = screenshotEntity->getScreenshotUrl();

                if (loadedScreenshots.contains(URL)) {
                    this->fullSizedImageLoaded(URL,
                                               loadedScreenshots.value(URL));
                    // And we are done
                    break;
                }

                // Download image
                if (!_resourceDownloadStreamToMemory) {
                    _resourceDownloadStreamToMemory = AW::Client::Communication::AWResourceDownloadStreamToMemoryFactory::createDefaultResourceDownloadStreamToMemory(NULL);
                    connect(_resourceDownloadStreamToMemory, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(networkResourceRequestsFinished()));
                }

                _resourceDownloadStreamToMemory->addResourceDownloadRequest(URL);
                connect (_resourceDownloadStreamToMemory, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), this, SLOT(fullSizedImageLoaded(QUrl,QByteArray)));

                _resourceDownloadStreamToMemory->startDownloadingResources();

                break;
            }
        }
    }
    else {
        // video

        QStringList elements = url.split("/");
        QString youTubeVideoID = elements.at(4);
        DLog("Youtube id: ") << youTubeVideoID;

//        if (fullSizedVideo->isHidden()) {
//            fullSizedImage->hide();
//            fullSizedVideo->show();
//        }

        if(ConfigManager::getIsEnableFlashInContents())
        {
            DLog("Flash enabled");
            MagnetWidget *videoMagnetWidget = dynamic_cast<MagnetWidget *>( _currFullSizeMediaPresenterWidget );
            QWebView *fullSizedVideo = NULL;
            if (videoMagnetWidget == NULL) {
                // remove the current widget, which is an image-presenter and replace it with a webview widget
                _relatedFullSizeMediaContainerLayout->removeWidget(_currFullSizeMediaPresenterWidget);
                _currFullSizeMediaPresenterWidget->deleteLater();

                videoMagnetWidget = new MagnetWidget;
                videoMagnetWidget->setFixedSize(550, 344);
            }
            else {
                fullSizedVideo = dynamic_cast<QWebView *>( videoMagnetWidget->getMagnetTargetWidget() );
            }

            if(fullSizedVideo == NULL) {
                fullSizedVideo = new QWebView();
                //            fullSizedVideo->setFixedSize(550,344);
                fullSizedVideo->settings()->setAttribute(QWebSettings::PluginsEnabled, true);

                fullSizedVideo->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
                fullSizedVideo->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
                fullSizedVideo->hide();

                //            fullSizedVideo->setHtml("");
                //            fullSizedVideo->setHtml("");
                //            fullSizedVideo->hide();

                videoMagnetWidget->setMagnetTargetWidget(fullSizedVideo, true, true, true);

                _currFullSizeMediaPresenterWidget = videoMagnetWidget;
                _relatedFullSizeMediaContainerLayout->addWidget(_currFullSizeMediaPresenterWidget, 0, Qt::AlignCenter);
            }

            //        _currFullSizeMediaPresenterWidget = fullSizedVideo;
            //        _relatedFullSizeMediaContainerLayout->addWidget(fullSizedVideo, 0, Qt::AlignCenter);

            //        fullSizedVideo->show();


            //        fullSizedVideo->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
            //        fullSizedVideo->show();
            fullSizedVideo->setHtml("<body style=\"margin:0; padding:0;\"><iframe src=\"http://www.youtube.com/embed/" + youTubeVideoID + "?rel=0\" width=550 height=343 frameborder=0></iframe><body>");
            //        fullSizedVideo->setHtml("<iframe src=\"http://www.youtube.com/embed/" + youTubeVideoID + "?rel=0\" width=550 height=343 frameborder=0></iframe>");
            //        fullSizedVideo->load(QUrl("http://www.youtube.com/embed/" + youTubeVideoID));
            //        fullSizedVideo->load(QUrl("http://developer.qt.nokia.com/forums/viewthread/4764"));
            //        fullSizedVideo->update();
            //        fullSizedVideo->repaint();
            //        fullSizedVideo->reload();
            //        fullSizedVideo->setFocusPolicy(Qt::StrongFocus);



            videoMagnetWidget->updateMagnetTargetGeometry();

            //        if(videoMagnetWidget->isVisible()) {
            //            fullSizedVideo->show();
            //            fullSizedVideo->raise();
            //        }
        }

        else
        {
            DLog("Flash disabled");
            // flash disabled
            QWebView *tmpWebView = dynamic_cast<QWebView *>( _currFullSizeMediaPresenterWidget );
            if (tmpWebView == NULL) {
                // remove the current widget, and replace it with a webview
                _relatedFullSizeMediaContainerLayout->removeWidget(_currFullSizeMediaPresenterWidget);
                _currFullSizeMediaPresenterWidget->deleteLater();

                tmpWebView = new QWebView;
                tmpWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
//                tmpWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
                tmpWebView->setFixedSize(550,344);

                _currFullSizeMediaPresenterWidget = tmpWebView;
                _relatedFullSizeMediaContainerLayout->addWidget(_currFullSizeMediaPresenterWidget, 0, Qt::AlignCenter);
            }

            tmpWebView->setHtml("<body style=\"margin:0; padding:0;\"><iframe src=\"http://www.youtube.com/embed/" + youTubeVideoID + "?rel=0\" width=550 height=343 frameborder=0></iframe><body>");

        }

    }
}

// Todo: URL kikeresese, ellenorzese h meg o kell e es betoltese
void ApplicationStoreApplicationPage::fullSizedImageLoaded(QUrl URL, QByteArray resource) {
    // Save it locally to not reload it
    loadedScreenshots.insert(URL.toString(), resource);

    SimpleImagePresenterWidget *fullSizedImage = dynamic_cast<SimpleImagePresenterWidget *>( _currFullSizeMediaPresenterWidget );
    if(fullSizedImage != NULL) {
        fullSizedImage->setImageByByteArray(resource, ImageTypes::PixelBased);
    }
    else {
        WLog("Image loaded, but the presenter widget is removed.");
    }
}


void ApplicationStoreApplicationPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

void ApplicationStoreApplicationPage::_showLicenceButtonPressed()
{
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("Specify a real licence URL, if any!");

    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    pageNavigationManager->pushPageToCurrentRootStackAndShow(new LicencePresenterPage("page-id-AppDetails-Licence-Subpage", _appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId()));
}

void ApplicationStoreApplicationPage::_openExternalLinkUrl(QString url)
{
    if(url.isEmpty()) {
        DLog("Empty URL given, cannot open it.");
        return;
    }

    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}
