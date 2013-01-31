#include "appiconandbuttonpresenterwidget.h"

// UI
#include <QVBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QPushButton>

#include "Common/Widgets/iconimagewidget.h"
#include "Common/Widgets/fadingbutton.h"
#include "Browser/Content/Widget/appsubstatespresenterwidget.h"

//
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QFileInfo>

#include "Common/imagetypesenum.h"
#include "Common/Helpers/pathhelper.h"
#include "Common/Event/startthisapplicationevent.h"

// utility
#include "Application/appwhirrapplication.h"
#include <QPainter>
#include "Common/Helpers/bestapplicationiconimageselectionhelper.h"
#include "Common/Helpers/qsizehelper.h"
#include "Common/System/systemprofiler.h"

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/globalobjectregistry.h"

#define BACKGROUND_TRESHOLD 10
#define ROUND_RECT_BORDER_WIDTH 1
#define ROUND_RECT_BORDER_ROUNDNESS 3

AppIconAndButtonPresenterWidget::AppIconAndButtonPresenterWidget(QString labelStyleSheet, QWidget *parent) :
    IdBasedSelectableAndHighlightableWidgetBase(parent),
    _backgroundColorMultiplier(0),
    _highlightRatio(0)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->_fadingItemDelegate = new FadingItemDelegate(this, GuiConfigurations::iconImageWidgetGrowShrinkAnimationDuration(), this);

    //setObjectName("myWidget");
    //setStyleSheet("QWidget#myWidget{ background-color : blue }");
//    QString css = "background-color: #ffA500;";
    //this->setAutoFillBackground(true);
//    setStyleSheet(css);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(10, 5, 10, 10);

    QWidget *iconAndTitleHolderWidget = new QWidget;
    QVBoxLayout *iconAndTitleLayout = new QVBoxLayout;
    iconAndTitleHolderWidget->setLayout(iconAndTitleLayout);

    mainLayout->addWidget(iconAndTitleHolderWidget);

    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    //this->setMinimumSize(GuiWidgetSizeRegistry::iconImagePresenterWidgetMinimumSize());
    //this->setMinimumSize(0, 0);

    //layout->setSizeConstraint(QLayout::SetFixedSize);


    //
    // child widgets

    QHBoxLayout *topPartLayout = new QHBoxLayout;
    iconAndTitleLayout->addLayout(topPartLayout);

    //
    topPartLayout->addSpacerItem(new QSpacerItem(25, 20, QSizePolicy::Fixed, QSizePolicy::Expanding));
    topPartLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    // icon
    iconImageWidget = new IconImageWidget(AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled);
    iconImageWidget->setIsAutoResizeToImageSize(false);
    iconImageWidget->setFixedSize(GuiConfigurations::applicationIconImageWidgetShrinkedSize());
    connect(iconImageWidget, SIGNAL(clicked()), this, SLOT(_iconImageClickedSlot()));
    topPartLayout->addWidget(iconImageWidget/*, 0, Qt::AlignHCenter*/);
    //
    topPartLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    // sub-state icons
    _appSubStatePresenterWidget = new AppSubStatesPresenterWidget;
    _appSubStatePresenterWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _appSubStatePresenterWidget->setFixedWidth(20);
    _appSubStatePresenterWidget->setContentsMargins(0, 5, 5, 0);
    connect(_appSubStatePresenterWidget, SIGNAL(notAvailableForThisPlatformIconPresenterActivated()), this, SLOT(_notAvailableForThisPlatformSubStateIconActivated()));
    connect(_appSubStatePresenterWidget, SIGNAL(removedFromStoreIconPresenterActivated()), this, SLOT(_removedFromStoreSubStateIconActivated()));
    connect(_appSubStatePresenterWidget, SIGNAL(updateAvailableIconPresenterActivated()), this, SLOT(_updateSubStateIconActivated()));
    topPartLayout->addWidget(_appSubStatePresenterWidget, 0, Qt::AlignTop | Qt::AlignRight);

//    iconAndTitleLayout->addWidget(iconImageWidget);



    //iconImageWidget->setMinimumSize(GuiWidgetSizeRegistry::iconImageWidgetShrinkedSize());
    //iconImageWidget->setMaximumSize(150, 150);
    //iconImageWidget->setFixedSize(100, 100);
//    iconImageWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    iconImageWidget->installEventFilter(this);

    //iconImageWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    //connect(iconImageWidget, SIGNAL())
    //iconImageWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    iconAndTitleLayout->setAlignment(iconImageWidget, Qt::AlignCenter);

    //QString css = "background-color: #ffA500;";
    //this->setAutoFillBackground(true);
    //iconImageWidget->setStyleSheet(css);


    titleLabel = new QLabel;
    titleLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    titleLabel->setStyleSheet(labelStyleSheet);
    iconAndTitleLayout->addWidget(titleLabel);
    iconAndTitleLayout->setAlignment(titleLabel, Qt::AlignCenter);

    //iconImageWidget->setMaximumSize(50, 50);

    QHBoxLayout *actionButtonLayout = new QHBoxLayout;
    iconAndTitleLayout->addLayout(actionButtonLayout);

//    actionButtonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
//    runButton = new QPushButton(tr("Install"));
    _runButton = new FadingButton;
    _runButton->setText(tr("Install"));
    _runButton->setStyleSheet(guiConfManager->getStyleForId("Button/RunApp/style"));
//    runButton->setMinimumSize(GuiConfigurations::appRunButtonMinimumSize());
//    runButton->setFixedSize(GuiConfigurations::appRunButtonMinimumSize());
//    runButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _runButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _runButton->setFixedHeight(guiConfManager->getIntForId("Button/RunApp/fixedHeight"));
    _runButton->setMinimumWidth(guiConfManager->getIntForId("Button/RunApp/minimumWidth"));
    _runButton->setFlat(true);
    connect(_runButton, SIGNAL(clicked()), this, SLOT(_mainAppStateDependentOptionActivated()));
    actionButtonLayout->addWidget(_runButton);
//    runButton->hide();

//    actionButtonLayout->addSpacerItem(new QSpacerItem(5, 0));
    actionButtonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    _manageAppButton = new FadingButton;
    _manageAppButton->setText(tr("Manage"));
    _manageAppButton->setStyleSheet(guiConfManager->getStyleForId("Button/ManageApp/style"));
    _manageAppButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _manageAppButton->setFixedHeight(guiConfManager->getIntForId("Button/ManageApp/fixedHeight"));
    _manageAppButton->setMinimumWidth(guiConfManager->getIntForId("Button/ManageApp/minimumWidth"));
    _manageAppButton->setFlat(true);
    connect(_manageAppButton, SIGNAL(clicked()), this, SLOT(_manageAppButtonActivated()));
    actionButtonLayout->addWidget(_manageAppButton);

//    actionButtonLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    _runButton->adjustSize();
    _manageAppButton->adjustSize();


    //
    // and add some animations

    // grow / shrink
    QStateMachine *machine = new QStateMachine();
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);

//    state1->assignProperty(iconImageWidget, "minimumSize", GuiConfigurations::applicationIconImageWidgetShrinkedSize());
//    state2->assignProperty(iconImageWidget, "minimumSize", GuiConfigurations::applicationIconImageWidgetGrownSize());

    state1->assignProperty(this, "highlightRatio", 0);
    state2->assignProperty(this, "highlightRatio", 1);

    state1->addTransition(this, SIGNAL(_startHighlightedAnimationSignal()), state2);
    state2->addTransition(this, SIGNAL(_startBackFromHighlightedToNormalAnimationSignal()), state1);


    // and add a default animation
//    QPropertyAnimation *defaultAnim = new QPropertyAnimation(iconImageWidget, "minimumSize");
    QPropertyAnimation *defaultAnim = new QPropertyAnimation(this, "highlightRatio");
    defaultAnim->setDuration(GuiConfigurations::iconImageWidgetGrowShrinkAnimationDuration());
    defaultAnim->setEasingCurve(QEasingCurve::Linear); // QEasingCurve::OutExpo
    machine->addDefaultAnimation(defaultAnim);

    machine->setInitialState(state1);
    machine->start();


    // --
    // rotate
    QStateMachine *rotateStateMachine = new QStateMachine();
    QState *rotateState1 = new QState(rotateStateMachine);
    QState *rotateState2 = new QState(rotateStateMachine);

    rotateState1->assignProperty(iconImageWidget, "rotationValue", 0.0f);
    rotateState2->assignProperty(iconImageWidget, "rotationValue", 1.0f);

    rotateState1->addTransition(this, SIGNAL(_startActivatedAnimationSignal()), rotateState2);
    rotateState2->addTransition(this, SIGNAL(_startBackFromActivatedToDefaultAnimationSignal()), rotateState1);

    connect(rotateState2, SIGNAL(propertiesAssigned()), this, SLOT(activatedAnimationEnded()));
    connect(rotateState1, SIGNAL(propertiesAssigned()), this, SLOT(backFromActivatedToDefaultAnimationEnded()));


    // and add a default animation
    QPropertyAnimation *defaultRotationAnim = new QPropertyAnimation(iconImageWidget, "rotationValue");
    defaultRotationAnim->setDuration(GuiConfigurations::iconImageWidgetRotateAnimationDuration());
    defaultRotationAnim->setEasingCurve(QEasingCurve::OutExpo); // InOutQuad, OutExpo
    rotateStateMachine->addDefaultAnimation(defaultRotationAnim);

    rotateStateMachine->setInitialState(rotateState1);
    rotateStateMachine->start();

    //
    // other

#if 0
    this->setMinimumSize(GuiConfigurations::zeroSize());
    this->setMaximumSize(GuiConfigurations::zeroSize());
#else
    //this->setMinimumSize();
    this->setMinimumSize(GuiConfigurations::applicationIconPresenterWithButtonWidgetFixedSize());
    this->setMaximumSize(GuiConfigurations::applicationIconPresenterWithButtonWidgetFixedSize());
    //this->setFixedSize(GuiConfigurations::iconImagePresenterWidgetNormalSize());
    //this->setVisible(true);
    //this->show();
#endif

}

void AppIconAndButtonPresenterWidget::installEventFilterOnIconImage(QObject *filterTarget)
{
    this->iconImageWidget->installEventFilter(filterTarget);
}

QString AppIconAndButtonPresenterWidget::getId() const
{
//    return this->iconModel.getStaticDescription()->getAppName();
    return this->getRelatedAppId();
//    return this->titleLabel->text(); // return the app-name as ID
}

QSize AppIconAndButtonPresenterWidget::sizeHint() const
{
    //static QSize s(100, 100);
    //return s;
    //return GuiConfigurations::iconImagePresenterWidgetSizeHint();
    return GuiConfigurations::applicationIconPresenterWithButtonWidgetFixedSize();
}

// [Q_SLOT]
void AppIconAndButtonPresenterWidget::activatedAnimationEnded()
{

//    Q_EMIT activated(this);

    if(this->iconImageWidget->getRelatedAppState() == AppWhirr::ApplicationGrabStates::Installed) {
        Q_EMIT runOptionActivated(this);
    }
    else {
        this->_runButton->setEnabled(false);
        Q_EMIT installOptionActivated(this);
    }

    this->startDeactivate();
}

void AppIconAndButtonPresenterWidget::backFromActivatedToDefaultAnimationEnded()
{
}

void AppIconAndButtonPresenterWidget::_mainAppStateDependentOptionActivated()
{
    this->startActivate();
//    if(this->iconImageWidget->getRelatedAppState() == AppWhirr::InstalledButUpdateAvailable || this->iconImageWidget->getRelatedAppState() == AppWhirr::NewestVersionInstalled) {
//        Q_EMIT runOptionActivated(this);
//    }
//    else {
//        this->runButton->setEnabled(false);
//        Q_EMIT installOptionActivated(this->_relatedAppId);
    //    }
}

void AppIconAndButtonPresenterWidget::_manageAppButtonActivated()
{
    Q_EMIT manageOptionActivated(this);
}

void AppIconAndButtonPresenterWidget::_iconImageClickedSlot()
{
//    Q_EMIT activated(this);
    Q_EMIT clicked(this);
}

void AppIconAndButtonPresenterWidget::_updateSubStateIconActivated() {
    Q_EMIT updateSubStateIconActivated(this);
}

void AppIconAndButtonPresenterWidget::_notAvailableForThisPlatformSubStateIconActivated() {
    Q_EMIT notAvailableForThisPlatformSubStateIconActivated(this);
}

void AppIconAndButtonPresenterWidget::_removedFromStoreSubStateIconActivated() {
    Q_EMIT removedFromStoreSubStateIconActivated(this);
}

void AppIconAndButtonPresenterWidget::startHighlighted()
{
    this->grow();
}

void AppIconAndButtonPresenterWidget::startNotHighlighted()
{
    this->shrink();
}

void AppIconAndButtonPresenterWidget::startActivate()
{
    //if(!showAnimationInProgressFlag && !hideAnimationInProgressFlag) {
        Q_EMIT _startActivatedAnimationSignal();
    //}
}

void AppIconAndButtonPresenterWidget::startDeactivate()
{
    //if(!showAnimationInProgressFlag && !hideAnimationInProgressFlag) {
        Q_EMIT _startBackFromActivatedToDefaultAnimationSignal();
    //}
}

void AppIconAndButtonPresenterWidget::grow()
{
#if 0
    if(!showAnimationInProgressFlag && !hideAnimationInProgressFlag) {
        Q_EMIT _growSignal();
    }
#endif

    Q_EMIT _startHighlightedAnimationSignal();
//    this->startFadeInAnimation();
}

void AppIconAndButtonPresenterWidget::shrink()
{
#if 0
    if(!showAnimationInProgressFlag && !hideAnimationInProgressFlag) {
        Q_EMIT _shrinkSignal();
    }
#endif

    Q_EMIT _startBackFromHighlightedToNormalAnimationSignal();
//    this->startFadeOutAnimation();
}

void AppIconAndButtonPresenterWidget::paintEvent(QPaintEvent *e)
{
//    QPainter painter(this);
//    painter.fillRect(this->rect(), QColor(150, 150, 150, (int)(255.0f * this->getFadeValue())));


//    this->_fadingItemDelegate->delegatePaintEventHere(e);
    this->performNormalPaint(e);
}

void AppIconAndButtonPresenterWidget::mouseEnteredEvent()
{
//    this->_runButton->show();
    this->_runButton->showWithAnimation();
    _manageAppButton->showWithAnimation();
    Q_EMIT mouseEntered(this);
}

void AppIconAndButtonPresenterWidget::mouseLeavedEvent()
{
//    this->_runButton->hide();
    this->_runButton->hideWithAnimation();
    _manageAppButton->hideWithAnimation();
    Q_EMIT mouseLeaved(this);
}

bool AppIconAndButtonPresenterWidget::event(QEvent *e)
{
    if(e->type() == QEvent::MouseButtonRelease)
    {
//        this->startActivate();
        Q_EMIT clicked(this);
        return true;
    }
    else if(e->type() == QEvent::Enter)
    {
        this->mouseEnteredEvent();
        return true;
    }
    else if(e->type() == QEvent::Leave)
    {
        this->mouseLeavedEvent();
        return true;
    }

    return IdBasedSelectableAndHighlightableWidgetBase::event(e);
}

//void AppIconAndButtonPresenterWidget::fadeValueChanged()
//{
//    this->repaint();
//}

void AppIconAndButtonPresenterWidget::setIconState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newState)
{
    this->iconImageWidget->setRelatedAppState(newState);
}

void AppIconAndButtonPresenterWidget::_setContent(QString appId, QString appName, QString iconFilePath, ImageTypes::ImageTypesEnum imageType, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState, AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags appSubStates)
{
//    this->_relatedAppId = appId;
    titleLabel->setText(appName);
    iconImageWidget->setImageByPath(iconFilePath, imageType);

    _appSubStatePresenterWidget->setSubStates(appSubStates);

    this->refreshContentByNewAppState(appState);
}

void AppIconAndButtonPresenterWidget::refreshContentByNewAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->_relatedAppDescriptionModel.getDynamicDescription_DataForModification()->setAppState(newAppState);

    if(!_relatedAppDescriptionModel.isCompatibleWithTheCurrentPlatform()) {
        this->_runButton->setText(tr("Alternatives"));
        this->_runButton->setEnabled(false);
        this->setIconState(AppWhirr::ApplicationGrabStates::GrabbedAndInstalling);

        _runButton->setStyleSheet(guiConfManager->getStyleForId("Button/RunApp/SecondOption/style"));
    }

    else {
        _runButton->setStyleSheet(guiConfManager->getStyleForId("Button/RunApp/style"));
        if( ((int)newAppState) < ((int)AppWhirr::ApplicationGrabStates::Installed) ) {
            // not yet installed
            if( newAppState == AppWhirr::ApplicationGrabStates::GrabbedAndInstalling ) {
                this->_runButton->setText(tr("Installing..."));
                this->_runButton->setEnabled(false);
                this->setIconState(AppWhirr::ApplicationGrabStates::GrabbedAndInstalling);
            }
            else if( newAppState == AppWhirr::ApplicationGrabStates::GrabbedAndUninstalling) {
                this->_runButton->setText(tr("Uninstalling..."));
                this->_runButton->setEnabled(false);
                this->setIconState(AppWhirr::ApplicationGrabStates::GrabbedAndUninstalling);
            }
            else {
                this->_runButton->setText(tr("Install"));
                this->setIconState(AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled);
                this->_runButton->setEnabled(true);
            }
        } else {
            // already installed
            if(newAppState == AppWhirr::ApplicationGrabStates::InstalledAndUpdating) {
                this->_runButton->setText(tr("Updating..."));
                this->_runButton->setEnabled(false);
                this->setIconState(AppWhirr::ApplicationGrabStates::InstalledAndUpdating);
            }
            else {
                if(this->_relatedAppDescriptionModel.isUpdateAvailable()) {
                    this->_runButton->setText(tr("Update"));
                    this->setIconState(AppWhirr::ApplicationGrabStates::Installed);
                }
                else {
                    this->_runButton->setText(tr("Run"));
                    this->setIconState(AppWhirr::ApplicationGrabStates::Installed);
                }

                this->_runButton->setEnabled(true);
            }
        }
    }

    this->_runButton->adjustSize();
}

void AppIconAndButtonPresenterWidget::setContentByAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel)
{
    this->_relatedAppDescriptionModel = appDescriptionModel;

//    if(!iconModel.getStaticDescription()->getIsIconRelatedDataValid())
//    {
//        qDebug("invalid icon model");
//        //return;
//    }

//    this->iconModel = iconModel;

//    this->_relatedAppId = ;

//    titleLabel->setText();

    DLog("Refresh app-icon-presenter: ") << appDescriptionModel.getDynamicDescription_ReadOnly()->getAppName() << appDescriptionModel.getDynamicDescription_ReadOnly()->getGrabbedIconPath() << appDescriptionModel.getStaticDescription_ReadOnly()->getLargeIconPath();

    ImageWithTypeInfo bestImageWithTypeInfo = BestApplicationIconImageSelectionHelper::getBestIconImageFromAppDescription(this->_relatedAppDescriptionModel, ImageTypes::SvgBased, BestApplicationIconImageSelectionHelper::Biggest);
    if(bestImageWithTypeInfo.getImagePath().isEmpty()) {
        DLog("Cannot select icon image.");
    }
    else {
//        iconImageWidget->setImageByPath(bestImageWithTypeInfo.getImagePath(), bestImageWithTypeInfo.getImageType());
        AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags appSubStates = AppWhirr::ApplicationSubStateFlags::NoState;
        if(_relatedAppDescriptionModel.isUpdateAvailable()) {
            appSubStates |= AppWhirr::ApplicationSubStateFlags::UpdateAvailable;
        }
        if(!_relatedAppDescriptionModel.isCompatibleWithTheCurrentPlatform()) {
            appSubStates |= AppWhirr::ApplicationSubStateFlags::NotAvailableOnThisPlatform;
        }
        if(_relatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppStatusInStore() == AppWhirr::ApplicationStatusInStore::RemovedFromStore) {
            appSubStates |= AppWhirr::ApplicationSubStateFlags::AppRemovedFromTheSystem;
        }

        this->_setContent(_relatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), _relatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName(), bestImageWithTypeInfo.getImagePath(), bestImageWithTypeInfo.getImageType(), _relatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState(), appSubStates);
    }
}

//ApplicationDescriptionModel AppIconAndButtonPresenterWidget::getIconModel() const
//{
//    return this->iconModel;
//}

ApplicationDescriptionModel AppIconAndButtonPresenterWidget::getRelatedAppDescriptionModel() {
    return this->_relatedAppDescriptionModel;
}

QString AppIconAndButtonPresenterWidget::getRelatedAppId() const {
    return this->_relatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId();
}

QString AppIconAndButtonPresenterWidget::getRelatedAppName() const {
    return this->_relatedAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName();
}

IconImageWidget *AppIconAndButtonPresenterWidget::getIconImageWidget() const
{
    return this->iconImageWidget;
}

// --------------------------------------------------
// --- FadingItemInterface implementation methods ---
void AppIconAndButtonPresenterWidget::startFadeInAnimation()
{
    DLog("Start fade IN anim");
    this->_fadingItemDelegate->startFadeInAnimation();
}

void AppIconAndButtonPresenterWidget::startFadeOutAnimation()
{
    DLog("Start fade OUT anim");
    this->_fadingItemDelegate->startFadeOutAnimation();
}

// ---------------------------------------------------------------
// --- FadingPreformerDelegateInterface implementation methods ---

void AppIconAndButtonPresenterWidget::performNormalPaint(QPaintEvent *e)
{    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

#if 0 // with color transition
//    DLog("----- paint");
    int backgroundBaseColorComp = 230;
    int backgroundHighlightedColorComp = 200;
    int backgroundCurrColorComp = backgroundBaseColorComp + ((backgroundHighlightedColorComp - backgroundBaseColorComp) * this->_backgroundColorMultiplier);

    int borderBaseColorComp = 210;
    int borderHighlightedColorComp = 180;
    int borderCurrColorComp = borderBaseColorComp + ((borderHighlightedColorComp - borderBaseColorComp) * this->_backgroundColorMultiplier);
//    int borderCurrColorComp = borderHighlightedColorComp * this->_backgroundColorMultiplier;

//    painter.fillRect(this->rect(), QColor(currColorComp, currColorComp, currColorComp));


    QPainterPath path;
    path.addRoundedRect(outerRect, ROUND_RECT_BORDER_ROUNDNESS, ROUND_RECT_BORDER_ROUNDNESS);
    painter.fillPath(path, QColor(borderCurrColorComp, borderCurrColorComp, borderCurrColorComp));
#endif

#if 1 // with transparency

    if(this->_backgroundColorMultiplier < 0.1f) {
        // don't do anything
        return;
    }

    painter.setOpacity(this->_backgroundColorMultiplier * 0.8);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    QColor borderColor(guiConfManager->getStyleForId("ColorHex/Default/DarkGray/Highlighted"));
    QColor backgroundColor(guiConfManager->getStyleForId("ColorHex/Default/LightGray/Highlighted"));
    QRect borderRect = QRect(this->rect().left() + BACKGROUND_TRESHOLD, this->rect().top() + BACKGROUND_TRESHOLD, this->rect().width() - 2*BACKGROUND_TRESHOLD, this->rect().height() - 2*BACKGROUND_TRESHOLD);

//    QRect backgroundRect = QRect(borderRect.left() + ROUND_RECT_BORDER_WIDTH, borderRect.top() + ROUND_RECT_BORDER_WIDTH, borderRect.width() - 2*ROUND_RECT_BORDER_WIDTH, borderRect.height() - 2*ROUND_RECT_BORDER_WIDTH);
    QRect backgroundRect = borderRect;

    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(backgroundRect, ROUND_RECT_BORDER_ROUNDNESS, ROUND_RECT_BORDER_ROUNDNESS);
    painter.fillPath(backgroundPath, backgroundColor);

    QPainterPath borderPath;
    borderPath.addRoundedRect(borderRect, ROUND_RECT_BORDER_ROUNDNESS, ROUND_RECT_BORDER_ROUNDNESS);
    QPen borderPen(borderColor, ROUND_RECT_BORDER_WIDTH);
    painter.setPen(borderPen);
    painter.drawPath(borderPath);
#endif

//    IdBasedSelectableAndHighlightableWidgetBase::paintEvent(e);
}

void AppIconAndButtonPresenterWidget::performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e)
{
    QPainter painter(this);
    painter.setOpacity(requiredOpacity);
    painter.drawPixmap(this->rect(), pixmap);
}

QWidget *AppIconAndButtonPresenterWidget::getPresenterWidget()
{
    return this;
}

float AppIconAndButtonPresenterWidget::getBackgroundColorMultiplier() const
{
    return this->_backgroundColorMultiplier;
}

void AppIconAndButtonPresenterWidget::setBackgroundColorMultiplier(float value)
{
    this->_backgroundColorMultiplier = value;
}

float AppIconAndButtonPresenterWidget::getHighlightRatio() const {
    return this->_highlightRatio;
}

void AppIconAndButtonPresenterWidget::setHighlightRatio(float value) {
    this->_highlightRatio = value;
    this->setBackgroundColorMultiplier(value);


    QSize baseSize = GuiConfigurations::applicationIconImageWidgetShrinkedSize();
    QSize currentSize = baseSize + QSizeHelper::multiplyQSizeWithNumber(GuiConfigurations::applicationIconImageWidgetGrownSize() - baseSize, this->_highlightRatio);

//    DLog("Curr size: ") << currentSize;

    iconImageWidget->setMinimumSize(currentSize);
    this->update();
}
