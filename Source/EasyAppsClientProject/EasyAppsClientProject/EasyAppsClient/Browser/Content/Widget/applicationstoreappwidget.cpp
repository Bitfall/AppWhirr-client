#include "applicationstoreappwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QThreadPool>

#include <QStateMachine>
#include <QPropertyAnimation>

#include <QVBoxLayout>
#include "Common/Widgets/fadingbutton.h"

// utility
#include "Common/Runnables/runnable_qimagefromresourceloader.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Application/globalobjectregistry.h"
#include "Common/Helpers/platformflagshelper.h"
#include "Common/Helpers/qcolorhelper.h"

// config
#include "Common/guiconfigurationmanager.h"

#define BACKGROUND_TRESHOLD 3
#define ROUND_RECT_BORDER_WIDTH 1
#define ROUND_RECT_BORDER_ROUNDNESS 3
#define CONTENT_PADDING 5

ApplicationStoreAppWidget::ApplicationStoreAppWidget(ApplicationBaseInfosNetworkEntityModel applicationListEntity, QWidget *parent) :
    IdBasedSelectableAndHighlightableWidgetBase(parent)
{
    ApplicationDescriptionModel::DynamicDescriptionModel *dynamicAppDescription = new ApplicationDescriptionModel::DynamicDescriptionModel;
    dynamicAppDescription->setAppId(applicationListEntity.getApplicationID());
    dynamicAppDescription->setAppName(applicationListEntity.getApplicationName());
    dynamicAppDescription->setAppState(AppWhirr::ApplicationGrabStates::NoState);
    dynamicAppDescription->setNewestAvailableVersionForThisPlatform(applicationListEntity.getApplicationVersion());
    dynamicAppDescription->setSupportedPlatformFlags(PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(applicationListEntity.getApplicationVersionSummedFlags()));
    dynamicAppDescription->setAppStatusInStore(applicationListEntity.getAppStatusInStore());
//    this->_appDescrModel.setDynamicDescription(dynamicAppDescription);

    ApplicationDescriptionModel appDescriptionModelByNetworkEntity;
    appDescriptionModelByNetworkEntity.setDynamicDescription(dynamicAppDescription);

    // initialize
    this->__initialize(appDescriptionModelByNetworkEntity, applicationListEntity.getApplicationCategory(), applicationListEntity.getApplicationIconURL());
}

ApplicationStoreAppWidget::ApplicationStoreAppWidget(ApplicationDescriptionModel relatedAppDescriptionModel, QString category, QString iconUrl, QWidget *parent) :
    IdBasedSelectableAndHighlightableWidgetBase(parent)
{
    this->__initialize(relatedAppDescriptionModel, category, iconUrl);
}

void ApplicationStoreAppWidget::__initialize(ApplicationDescriptionModel relatedAppDescriptionModel, QString category, QString iconUrl)
{
    _highlightRatio = 0;
    _currAppGrabType = AGT_NotGrabbed;
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setFixedSize(220, 80);

    this->_appDescrModel = relatedAppDescriptionModel;

    this->_appCategory = category;
    this->_appIconUrl = iconUrl;


    _backgroundColor = guiConfManager->getColorForId("ColorHex/Default/LightGray/WidgetBackground");
    _highlightedBackgroundColor = guiConfManager->getColorForId("ColorHex/Default/LightGray/Highlighted");

    _borderColor = guiConfManager->getColorForId("ColorHex/Default/DarkGray/Default");
    _highlightedBorderColor = guiConfManager->getColorForId("ColorHex/Default/DarkGray/Highlighted");



    DLog("--- test newest version: ") << this->_appDescrModel.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform();
    DLog("--- supported platform flags: ") << this->_appDescrModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags();
//    _relatedAppState = GlobalObjectManager::sharedGlobalObjectRegistry()->getLocalApplicationDatabaseManager()->getAppStateById(this->_appId);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    _mainOptionButton = new FadingButton;
    _mainOptionButton->setText(tr("Grab"));
    _mainOptionButton->setStyleSheet(guiConfManager->getStyleForId("Button/RunApp/style"));
//    runButton->setMinimumSize(GuiConfigurations::appRunButtonMinimumSize());
//    runButton->setFixedSize(GuiConfigurations::appRunButtonMinimumSize());
//    runButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _mainOptionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _mainOptionButton->setFixedHeight(guiConfManager->getIntForId("Button/RunApp/fixedHeight"));
    _mainOptionButton->setMinimumWidth(guiConfManager->getIntForId("Button/RunApp/minimumWidth"));
    _mainOptionButton->setEnabled(false);
    _mainOptionButton->setFlat(true);
    connect(_mainOptionButton, SIGNAL(clicked()), this, SLOT(_mainAppStateDependentOptionActivated()));
    connect(this, SIGNAL(__startHighlightedAnimationSignal()), _mainOptionButton, SLOT(showWithAnimation()));
    connect(this, SIGNAL(__startBackFromHighlightedToNormalAnimationSignal()), _mainOptionButton, SLOT(hideWithAnimation()));
    mainLayout->addWidget(_mainOptionButton, 0, Qt::AlignBottom | Qt::AlignRight);


    QStateMachine *machine = new QStateMachine();
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);

    state1->assignProperty(this, "highlightRatio", 0);
    state2->assignProperty(this, "highlightRatio", 1);

    state1->addTransition(this, SIGNAL(__startHighlightedAnimationSignal()), state2);
    state2->addTransition(this, SIGNAL(__startBackFromHighlightedToNormalAnimationSignal()), state1);


    // and add a default animation
//    QPropertyAnimation *defaultAnim = new QPropertyAnimation(iconImageWidget, "minimumSize");
    QPropertyAnimation *defaultAnim = new QPropertyAnimation(this, "highlightRatio");
    defaultAnim->setDuration(guiConfManager->getIntForId("Store/MainPageWidget/Highlight/animationDuration"));
    defaultAnim->setEasingCurve(QEasingCurve::Linear); // QEasingCurve::OutExpo
    machine->addDefaultAnimation(defaultAnim);

    machine->setInitialState(state1);
    machine->start();

    this->_refreshByCurrAppDescription();
}

QString ApplicationStoreAppWidget::getId() const {
    return this->getRelatedAppId();
}

QString ApplicationStoreAppWidget::getRelatedAppName() const {
    return this->_appDescrModel.getDynamicDescription_ReadOnly()->getAppName();
}

QString ApplicationStoreAppWidget::getRelatedAppId() const {
    return this->_appDescrModel.getDynamicDescription_ReadOnly()->getAppId();
}

ApplicationDescriptionModel ApplicationStoreAppWidget::getRelatedAppDescription() const {
    return this->_appDescrModel;
}

//ApplicationListNetworkEntity* ApplicationStoreAppWidget::getEntity() {
//    return applicationListEntity;
//}

void ApplicationStoreAppWidget::retrievedResourceForURL(const QString& URL, const QByteArray& bytes) {
    if (this->_appIconUrl != URL) return;

    Runnable_QImageFromResourceLoader *iconImageLoader = new Runnable_QImageFromResourceLoader(bytes);
    connect(iconImageLoader, SIGNAL(imageLoaded(QImage)), this, SLOT(_iconImageLoaded(QImage)));
    QThreadPool::globalInstance()->start(iconImageLoader);
//    this->_iconImageLoaded(QImage::fromData(bytes));
}

void ApplicationStoreAppWidget::refresh(ApplicationDescriptionModel newAppDescription)
{
    this->_appDescrModel = newAppDescription;
    this->_refreshByCurrAppDescription();
}

void ApplicationStoreAppWidget::refreshByState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, bool isForceRefresh)
{
    if(!isForceRefresh) {
        if(newAppState == this->_appDescrModel.getDynamicDescription_ReadOnly()->getAppState()) {
            // do nothing
            return;
        }
    }

    this->_appDescrModel.getDynamicDescription_DataForModification()->setAppState(newAppState);
    this->_refreshByCurrAppDescription();
}

void ApplicationStoreAppWidget::_refreshByCurrAppDescription()
{
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState = this->_appDescrModel.getDynamicDescription_ReadOnly()->getAppState();
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    DLog("Refresh happened");
    if(((int)appState >= (int)AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled) && ((int)appState < (int)AppWhirr::ApplicationGrabStates::Installed)) {
//        this->_backgroundPixmap = QPixmap(":/ApplicationStoreAppIcon-Background-Grabbed");

//        _backgroundColor = guiConfManager->getColorForId("ColorHex/Default/LightGray/WidgetBackground");
//        _backgroundColor = QColor("#B5EAAF"); // about the 25% greener version of the default light-gray widget background
//        _highlightedBackgroundColor = QColor("#A0E896"); // about 10% greener

//        this->_turnOnGrabbedIndicatorIcon();
        this->_refreshGrabbedIndicatorIcon(AGT_Grabbed);
    }

    else if( (int)appState >= (int)AppWhirr::ApplicationGrabStates::Installed ) {
        this->_refreshGrabbedIndicatorIcon(AGT_GrabbedAndInstalled);
    }

    else {
//        this->_backgroundPixmap = QPixmap(":/ApplicationStoreAppIcon-Background-Default");
//        _backgroundColor = guiConfManager->getColorForId("ColorHex/Default/LightGray/WidgetBackground");
//        _highlightedBackgroundColor = guiConfManager->getColorForId("ColorHex/Default/LightGray/Highlighted");

        this->_refreshGrabbedIndicatorIcon(AGT_NotGrabbed);
//        this->_turnOffGrabbedIndicatorIcon();
    }

    //
    // refresh the button as well
    if(!_appDescrModel.isCompatibleWithTheCurrentPlatform()) {
        this->_mainOptionButton->setText(tr("Show alternative"));
        this->_mainOptionButton->setEnabled(false);
    }

    else {
        if( ((int)appState) < ((int)AppWhirr::ApplicationGrabStates::Installed) ) {
            // not yet installed
            if( appState == AppWhirr::ApplicationGrabStates::GrabbedAndInstalling ) {
                this->_mainOptionButton->setText(tr("Installing..."));
                this->_mainOptionButton->setEnabled(false);
            }
            else if( appState == AppWhirr::ApplicationGrabStates::GrabbedAndUninstalling) {
                this->_mainOptionButton->setText(tr("Uninstalling..."));
                this->_mainOptionButton->setEnabled(false);
            }
            else if( appState == AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled ) {
                this->_mainOptionButton->setText(tr("Install"));
                this->_mainOptionButton->setEnabled(true);
            }
            else {
                // not yet grabbed
                this->_mainOptionButton->setText(tr("Grab"));
                this->_mainOptionButton->setEnabled(true);
            }
        } else {
            // already installed
            if(appState == AppWhirr::ApplicationGrabStates::InstalledAndUpdating) {
                this->_mainOptionButton->setText(tr("Updating..."));
                this->_mainOptionButton->setEnabled(false);
            }
            else {
                if(this->_appDescrModel.isUpdateAvailable()) {
                    this->_mainOptionButton->setText(tr("Update"));
                }
                else {
                    this->_mainOptionButton->setText(tr("Run"));
                }
                this->_mainOptionButton->setEnabled(true);
            }
        }
    }

    this->_mainOptionButton->adjustSize();
    this->_mainOptionButton->update();

    this->update();
}

//void ApplicationStoreAppWidget::_turnOnGrabbedIndicatorIcon()
//{
//    if(_grabbedIndicatorIconPixmap.isNull()) {
//        _grabbedIndicatorIconPixmap = QPixmap(":/images/store/AppGrabbedIndicatorIcon");
//    }
//}

//void ApplicationStoreAppWidget::_turnOffGrabbedIndicatorIcon()
//{
//    if(!_grabbedIndicatorIconPixmap.isNull()) {
//        _grabbedIndicatorIconPixmap = QPixmap();
//    }
//}

void ApplicationStoreAppWidget::_refreshGrabbedIndicatorIcon(AppGrabTypeEnum appGrabType)
{
    if(appGrabType == _currAppGrabType) {
        return;
    }

    if(appGrabType == AGT_NotGrabbed) {
        _grabbedIndicatorIconPixmap = QPixmap(); // no grab indicator icon
    }
    else if(appGrabType == AGT_Grabbed) {
        _grabbedIndicatorIconPixmap = QPixmap(":/images/store/AppGrabbedIndicator-Grabbed");
    }
    else if(appGrabType == AGT_GrabbedAndInstalled) {
        _grabbedIndicatorIconPixmap = QPixmap(":/images/store/AppGrabbedIndicator-Installed");
    }
}

void ApplicationStoreAppWidget::_mainAppStateDependentOptionActivated()
{
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState = _appDescrModel.getDynamicDescription_ReadOnly()->getAppState();

    if(appState == AppWhirr::ApplicationGrabStates::Installed) {
//        if(this->_appDescriptionModel.isUpdateAvailable()) {
////            Q_EMIT updateThisApp(this->_appDescriptionModel);
//            Q_EMIT installThisApp(this->_appDescriptionModel);
//        }
//        else {
        // run
            Q_EMIT runThisApp(_appDescrModel);
//        }
    }
    else {
        // Install or update
        this->_mainOptionButton->setEnabled(false);
        Q_EMIT installOrUpdateThisApp(_appDescrModel);
    }
}

void ApplicationStoreAppWidget::_iconImageLoaded(QImage image)
{
    this->_iconPixmap = QPixmap::fromImage(image);
    this->update();
}

void ApplicationStoreAppWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    painter.setOpacity(0.9);

    //
    // --- background
    //    painter.drawPixmap(QRect(0, 20, 220, 50), this->_backgroundPixmap);

//    if(this->_highlightRatio < 0.1f) {
//        // don't do anything
//        return;
//    }

//    painter.setOpacity(this->_highlightRatio);
//    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

//    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
//    QColor borderColor(guiConfManager->getStyleForId("ColorHex/Default/DarkGray/Highlighted"));
//    QColor backgroundColor(guiConfManager->getStyleForId("ColorHex/Default/LightGray/Highlighted"));

    QColor currBackgroundColor = QColorHelper::blendColors(_backgroundColor, _highlightedBackgroundColor, this->_highlightRatio);

    QRect borderRect = QRect(this->rect().left() + BACKGROUND_TRESHOLD, this->rect().top() + BACKGROUND_TRESHOLD, this->rect().width() - 2*BACKGROUND_TRESHOLD, this->rect().height() - 2*BACKGROUND_TRESHOLD);

//    QRect backgroundRect = QRect(borderRect.left() + ROUND_RECT_BORDER_WIDTH, borderRect.top() + ROUND_RECT_BORDER_WIDTH, borderRect.width() - 2*ROUND_RECT_BORDER_WIDTH, borderRect.height() - 2*ROUND_RECT_BORDER_WIDTH);
    QRect backgroundRect = borderRect;

    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(backgroundRect, ROUND_RECT_BORDER_ROUNDNESS, ROUND_RECT_BORDER_ROUNDNESS);
    painter.fillPath(backgroundPath, currBackgroundColor);


    QColor currBorderColor = QColorHelper::blendColors(_borderColor, _highlightedBorderColor, this->_highlightRatio);
    QPainterPath borderPath;
    borderPath.addRoundedRect(borderRect, ROUND_RECT_BORDER_ROUNDNESS, ROUND_RECT_BORDER_ROUNDNESS);
    QPen borderPen(currBorderColor, ROUND_RECT_BORDER_WIDTH);
    painter.setPen(borderPen);
    painter.drawPath(borderPath);


    //
    // --- content
//    painter.setOpacity(1);
    int contentInset = BACKGROUND_TRESHOLD + ROUND_RECT_BORDER_WIDTH + CONTENT_PADDING;
    painter.drawPixmap(QRect(contentInset, contentInset, 80, 80), this->_iconPixmap);

    painter.setFont(QFont("Arial", 12));
    painter.setPen(QColor(87, 87, 87));
    painter.drawText(QRectF(90 + contentInset, 25 + contentInset, 135, 18), this->_appDescrModel.getDynamicDescription_ReadOnly()->getAppName());

    painter.setFont(QFont("Arial", 12));
    painter.setPen(QColor(157, 158, 158));
    painter.drawText(QRectF(90 + contentInset, 43 + contentInset, 135, 20), this->_appCategory);

    if(!_grabbedIndicatorIconPixmap.isNull()) {
        QPainterPath grabStateBackgroundPath;
        grabStateBackgroundPath.moveTo(QPointF(this->rect().width() - contentInset + CONTENT_PADDING, 55));
        grabStateBackgroundPath.lineTo(QPointF(this->rect().width() - contentInset - 60, 5));
        grabStateBackgroundPath.lineTo(QPointF(this->rect().width() - contentInset - 2, 5));
        grabStateBackgroundPath.lineTo(QPointF(this->rect().width() - contentInset + CONTENT_PADDING, 10));
        grabStateBackgroundPath.lineTo(QPointF(this->rect().width() - contentInset + CONTENT_PADDING, 65));
        QColor grabStateBackgroundColor = QColor(120, 120, 120, 50);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillPath(grabStateBackgroundPath, grabStateBackgroundColor);

        painter.drawPixmap(QRect(this->rect().width() - contentInset - 30, 5, 30, 30), _grabbedIndicatorIconPixmap);
    }
}

void ApplicationStoreAppWidget::mouseReleaseEvent (QMouseEvent* event) {
    Q_EMIT activated(this);
    Q_EMIT clicked(this);
}

void ApplicationStoreAppWidget::enterEvent(QEvent *e) {
    Q_EMIT __startHighlightedAnimationSignal();
    Q_EMIT mouseEntered(this);
    IdBasedSelectableAndHighlightableWidgetBase::enterEvent(e);
}

void ApplicationStoreAppWidget::leaveEvent(QEvent *e) {
    Q_EMIT __startBackFromHighlightedToNormalAnimationSignal();
    Q_EMIT mouseLeaved(this);
    IdBasedSelectableAndHighlightableWidgetBase::leaveEvent(e);
}

float ApplicationStoreAppWidget::getHighlightRatio() const {
    return this->_highlightRatio;
}

void ApplicationStoreAppWidget::setHighlightRatio(float value) {
    this->_highlightRatio = value;

//    QSize baseSize = GuiConfigurations::applicationIconImageWidgetShrinkedSize();
//    QSize currentSize = baseSize + QSizeHelper::multiplyQSizeWithNumber(GuiConfigurations::applicationIconImageWidgetGrownSize() - baseSize, this->_highlightRatio);

////    DLog("Curr size: ") << currentSize;

//    iconImageWidget->setMinimumSize(currentSize);
    this->update();
}
