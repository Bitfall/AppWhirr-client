#include "myappsmanagementsidebar.h"

// UI
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "Common/Widgets/simpleimagepresenterwidget.h"
#include "Common/Widgets/checkboxbutton.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"

// utility
#include <QSpacerItem>
#include "Common/Helpers/bestapplicationiconimageselectionhelper.h"
#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

MyAppsManagementSidebar::MyAppsManagementSidebar(QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, QWidget *parent) :
    AnimatedTwoFixedSizeWidgetBase(500, collapsedSize, expandedSize, isExpandedByDefault, true, false, parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
//    sidebarLayout->setAlignment(Qt::AlignTop);
    this->setLayout(mainLayout);
//    sidebarWidget->setMinimumWidth(240);
//    sidebarWidget->setMaximumWidth(240);
    this->setContentsMargins(5, 5, 5, 5);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setStyleSheet(guiConfManager->getStyleForId("MyApps/AppManagementSidebar/style"));

    QPushButton *hideSidebarButton = new QPushButton(tr("Hide"));
    connect(hideSidebarButton, SIGNAL(clicked()), this, SLOT(switchToCollapsedStateSlot()));
//    hideSidebarButton->setFixedHeight(30);
    hideSidebarButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    hideSidebarButton->setFlat(true);
    mainLayout->addWidget(hideSidebarButton);

    _titleLabel = new QLabel("App");
    _titleLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Large/DarkGray/style"));
    _titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(_titleLabel);

    _imagePresenter = new SimpleImagePresenterWidget;
    _imagePresenter->setFixedSize(150, 150);
    mainLayout->addWidget(_imagePresenter, 0, Qt::AlignHCenter);

    mainLayout->addSpacerItem(new QSpacerItem(0, 10));

    _updateAppButton = new QPushButton(tr("Update"));
    connect(_updateAppButton, SIGNAL(clicked()), this, SLOT(_updateButtonClickedSlot()));
    _updateAppButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _updateAppButton->setFlat(true);
    _updateAppButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/HighPriority/style"));
    mainLayout->addWidget(_updateAppButton);
    _updateAppButton->hide();

    _runAppButton = new QPushButton(tr("Run"));
    connect(_runAppButton, SIGNAL(clicked()), this, SLOT(_mainAppStateDependentButtonClickedSlot()));
    _runAppButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _runAppButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/HighPriority/style"));
    _runAppButton->setFlat(true);
    mainLayout->addWidget(_runAppButton);

    this->_showAppDetailsButton = new QPushButton(tr("Show details"));
    connect(_showAppDetailsButton, SIGNAL(clicked()), this, SLOT(_showAppDetailsButtonClickedSlot()));
    _showAppDetailsButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _showAppDetailsButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    _showAppDetailsButton->setFlat(true);
    mainLayout->addWidget(this->_showAppDetailsButton);

//    this->autostartCheckbox = new QCheckBox(tr("Start with AppWhirr"));
    this->_autostartCheckbox = new CheckboxButton(tr("Start with AppWhirr"));
    this->_autostartCheckbox->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
    this->_autostartCheckbox->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    connect(this->_autostartCheckbox, SIGNAL(toggled(bool)), this, SLOT(_autoStartThisAppToggled(bool)));
    mainLayout->addWidget(this->_autostartCheckbox);

//    this->isHiddenCheckbox = new QCheckBox(tr("Hide it from my main list"));
    this->_isHiddenCheckbox = new CheckboxButton(tr("Hide from main lists"));
    this->_isHiddenCheckbox->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/Gray/style"));
    this->_isHiddenCheckbox->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    connect(this->_isHiddenCheckbox, SIGNAL(toggled(bool)), this, SLOT(_isHideThisAppToggled(bool)));
    mainLayout->addWidget(this->_isHiddenCheckbox);

    _createDesktopShortcutButton = new QPushButton(tr("Create Desktop Shortcut"));
    connect(_createDesktopShortcutButton, SIGNAL(clicked()), this, SLOT(_createDesktopShortcutForThisAppButtonClickedSlot()));
    _createDesktopShortcutButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _createDesktopShortcutButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    _createDesktopShortcutButton->setFlat(true);
    mainLayout->addWidget(_createDesktopShortcutButton);

    _removeAppButton = new QPushButton(tr("Uninstall"));
    connect(_removeAppButton, SIGNAL(clicked()), this, SLOT(_removeThisAppButtonClickedSlot()));
    _removeAppButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _removeAppButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    _removeAppButton->setFlat(true);
    mainLayout->addWidget(_removeAppButton);

//    // spacer
//    QSpacerItem *spacer1 = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Fixed);
//    mainLayout->addSpacerItem(spacer1);

    // spacer
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addSpacerItem(spacer);

    _suggestUpdateAvailableButton = new QPushButton(tr("Suggest it's not the latest version!"));
    connect(_suggestUpdateAvailableButton, SIGNAL(clicked()), this, SLOT(suggestUpdateAvailable()));
    _suggestUpdateAvailableButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _suggestUpdateAvailableButton->setStyleSheet(guiConfManager->getStyleForId("Button/LowPriority/SmallText/style"));
    _suggestUpdateAvailableButton->setFlat(true);
    mainLayout->addWidget(_suggestUpdateAvailableButton);
    _suggestUpdateAvailableButton->hide();

    // spacer - bottom
    QSpacerItem *bottomSpacer = new QSpacerItem(0, 5, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(bottomSpacer);

//    this->setIsSetOnlyMinimumSize(true);
//    this->setIsSetHeight(false);

    LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();
    connect(localAppDatabaseManager, SIGNAL(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)), this, SLOT(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)));
    connect(localAppDatabaseManager, SIGNAL(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)), this, SLOT(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)));
}

void MyAppsManagementSidebar::setContent(ApplicationDescriptionModel appDescriptionModel, QPixmap mainImagePixmap)
{
    this->_appDescriptionModel = appDescriptionModel;

//    appState = localAppDatabaseManager->getAppStateById(appId);

    this->_refreshByCurrentAppDescription(mainImagePixmap);
}

QString MyAppsManagementSidebar::getRelatedAppId() const
{
    return this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId();
}

void MyAppsManagementSidebar::_refreshByCurrentAppDescription(QPixmap preferredImagePixmap)
{
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState = _appDescriptionModel.getDynamicDescription_ReadOnly()->getAppState();

//    this->_currAppId = appId;
    this->_titleLabel->setText(_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppName());

    bool isNewValidImageFoundAndApplied = false;
    if(!preferredImagePixmap.isNull()) {
        // given image is valid
        // apply the given image
        this->_imagePresenter->setImageByPixmap(preferredImagePixmap);
        isNewValidImageFoundAndApplied = true;
    }
    else /*if(appState == AppWhirr::NewestVersionInstalled || appState == AppWhirr::InstalledButUpdateAvailable)*/ {
        // we can get the image from the app - use it's icon image
        ImageWithTypeInfo bestImageWithTypeInfo = BestApplicationIconImageSelectionHelper::getBestIconImageFromAppDescription(this->_appDescriptionModel, ImageTypes::SvgBased, BestApplicationIconImageSelectionHelper::Biggest);
        if(bestImageWithTypeInfo.getImagePath().isEmpty()) {
            isNewValidImageFoundAndApplied = false;
        }
        else {
            this->_imagePresenter->setImageByPath(bestImageWithTypeInfo.getImagePath(), bestImageWithTypeInfo.getImageType());
            isNewValidImageFoundAndApplied = true;
        }
    }

    if(!isNewValidImageFoundAndApplied) {
        // apply the previous one, if any
        // -> it's already set, so don't do anything
    }

    //
    // check the autostartup state of the app
    this->_autostartCheckbox->setChecked(_appDescriptionModel.getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts());
    this->_isHiddenCheckbox->setChecked(_appDescriptionModel.getDynamicDescription_ReadOnly()->getIsHidden());

    if(!_appDescriptionModel.isCompatibleWithTheCurrentPlatform()) {
        this->_runAppButton->setText(tr("Show alternative"));
        this->_runAppButton->setEnabled(false);

        this->_updateAppButton->setEnabled(false);
        this->_updateAppButton->hide();

        this->_removeAppButton->setEnabled(false);
        this->_removeAppButton->hide();
        this->_suggestUpdateAvailableButton->setEnabled(false);
        this->_suggestUpdateAvailableButton->hide();
        this->_autostartCheckbox->setEnabled(false);
        this->_autostartCheckbox->hide();
        this->_createDesktopShortcutButton->setEnabled(false);
        this->_createDesktopShortcutButton->hide();
    }

    else {
        // actualize button states regarding app-state
        if(appState == AppWhirr::ApplicationGrabStates::Installed) {
            this->_runAppButton->setEnabled(true);
            //        this->_runAppButton->show();

            //        if(this->_appDescriptionModel.isUpdateAvailable()) {
            //            this->runAppButton->setText(tr("Update"));
            //        }
            //        else {
            this->_runAppButton->setText(tr("Run"));
            //        }

            if(_appDescriptionModel.isUpdateAvailable()) {
                this->_updateAppButton->setEnabled(true);
                this->_updateAppButton->show();
            }
            else {
                this->_updateAppButton->setEnabled(false);
                this->_updateAppButton->hide();
            }

            this->_removeAppButton->setEnabled(true);
            this->_removeAppButton->show();
            //
            this->_suggestUpdateAvailableButton->setEnabled(true);
            this->_suggestUpdateAvailableButton->show();
            //
            this->_autostartCheckbox->setEnabled(true);
            this->_autostartCheckbox->show();
            //
            this->_createDesktopShortcutButton->setEnabled(true);
            this->_createDesktopShortcutButton->show();
        }
        else {
            if(appState == AppWhirr::ApplicationGrabStates::GrabbedAndInstalling) {
                this->_runAppButton->setText(tr("Installing..."));
                this->_runAppButton->setEnabled(false);
                //            this->_runAppButton->hide();

                this->_updateAppButton->setEnabled(false);
                this->_updateAppButton->hide();

                this->_removeAppButton->setEnabled(false);
                this->_removeAppButton->hide();
                this->_suggestUpdateAvailableButton->setEnabled(false);
                this->_suggestUpdateAvailableButton->hide();
                this->_autostartCheckbox->setEnabled(false);
                this->_autostartCheckbox->hide();
                this->_createDesktopShortcutButton->setEnabled(false);
                this->_createDesktopShortcutButton->hide();
            }
            else if(appState == AppWhirr::ApplicationGrabStates::GrabbedAndUninstalling) {
                this->_runAppButton->setText(tr("Uninstalling..."));
                this->_runAppButton->setEnabled(false);
                //            this->_runAppButton->hide();

                this->_updateAppButton->setEnabled(false);
                this->_updateAppButton->hide();

                this->_removeAppButton->setEnabled(false);
                this->_removeAppButton->hide();
                this->_suggestUpdateAvailableButton->setEnabled(false);
                this->_suggestUpdateAvailableButton->hide();
                this->_autostartCheckbox->setEnabled(false);
                this->_autostartCheckbox->hide();
                this->_createDesktopShortcutButton->setEnabled(false);
                this->_createDesktopShortcutButton->hide();
            }
            else {
                this->_runAppButton->setEnabled(true);
                this->_runAppButton->show();
                this->_runAppButton->setText(tr("Install"));

                this->_updateAppButton->setEnabled(false);
                this->_updateAppButton->hide();

                this->_removeAppButton->setEnabled(false);
                this->_removeAppButton->hide();
                this->_suggestUpdateAvailableButton->setEnabled(false);
                this->_suggestUpdateAvailableButton->hide();
                this->_autostartCheckbox->setEnabled(false);
                this->_autostartCheckbox->hide();
                this->_createDesktopShortcutButton->setEnabled(false);
                this->_createDesktopShortcutButton->hide();
            }
        }
    }

//    this->update();
}

void MyAppsManagementSidebar::appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    if(appId == this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId()) {
        this->_appDescriptionModel.getDynamicDescription_DataForModification()->setAppState(newAppState);
        this->_refreshByCurrentAppDescription(QPixmap());
    }
}

void MyAppsManagementSidebar::wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel)
{
    if(updatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId() == this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId()) {
        this->_appDescriptionModel = updatedAppDescriptionModel;
        this->_refreshByCurrentAppDescription(QPixmap());
    }
}

//void MyAppsManagementSidebar::refreshCurrentContent()
//{
//    if(_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId().isEmpty()) {
//        // no app specified
//        return;
//    }

//    this->setContent(_appDescriptionModel, QPixmap());
//}

void MyAppsManagementSidebar::_autoStartThisAppToggled(bool newValue)
{
    this->_appDescriptionModel.getDynamicDescription_DataForModification()->setIsStartWhenClientStarts(newValue);

    Q_EMIT isStartThisAppWhenClientStartsChangedSignal(_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), newValue);
}

void MyAppsManagementSidebar::_isHideThisAppToggled(bool newValue)
{
    this->_appDescriptionModel.getDynamicDescription_DataForModification()->setIsHidden(newValue);

    Q_EMIT isThisAppHiddenChangedSignal(_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), newValue);
}

void MyAppsManagementSidebar::suggestUpdateAvailable()
{
    AppwhirrApplication::instance()->getBrowserWindow()->showFeedbackPage(tr("Hello AppWhirr Team!\n\nI would like to notify you that the version of the \"%1\" app available in AppWhirr is not the latest version of the app. Please update it in the AppWhirr app catalog!\n\nThank you guys!").arg(this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppName()));
}

void MyAppsManagementSidebar::_mainAppStateDependentButtonClickedSlot()
{
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState = _appDescriptionModel.getDynamicDescription_ReadOnly()->getAppState();

    if(appState == AppWhirr::ApplicationGrabStates::Installed) {
//        if(this->_appDescriptionModel.isUpdateAvailable()) {
////            Q_EMIT updateThisApp(this->_appDescriptionModel);
//            Q_EMIT installThisApp(this->_appDescriptionModel);
//        }
//        else {
            Q_EMIT runThisApp(_appDescriptionModel);
//        }
    }
    else {
        // Install
        this->_updateAppButton->setEnabled(false);
        this->_runAppButton->setEnabled(false);
        Q_EMIT installThisApp(_appDescriptionModel);
    }
}

void MyAppsManagementSidebar::_updateButtonClickedSlot()
{
    this->_updateAppButton->setEnabled(false);
    this->_runAppButton->setEnabled(false);
    Q_EMIT installThisApp(_appDescriptionModel);
}

void MyAppsManagementSidebar::_showAppDetailsButtonClickedSlot()
{
    Q_EMIT showAppDetails(_appDescriptionModel);
}

void MyAppsManagementSidebar::_removeThisAppButtonClickedSlot()
{
    Q_EMIT removeThisApp(_appDescriptionModel);
}

void MyAppsManagementSidebar::_createDesktopShortcutForThisAppButtonClickedSlot()
{
    Q_EMIT createDesktopShortcutForThisApp(_appDescriptionModel);
}
