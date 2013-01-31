#include "multisectionsamesizehorizontalflowlayoutbasedscrollarea.h"

// UI
#include <QScrollBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "sectionbasedhorizontalflowlayoutpresenterdelegate.h"
#include "../mousegrabscrolldelegate.h"

#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

//
#include <QWheelEvent>
#include <QMouseEvent>

MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea(SectionBasedHorizontalFlowLayoutPresenterDelegate *delegate, int sectionCount, QWidget *parent) :
    QScrollArea(parent),
    _delegate(delegate)
{
    this->setWidgetResizable(false);

    _mainContentHolderWidget = new QWidget;
//    _mainContentHolderWidget->setStyleSheet("QWidget { background-color: #f000f0; }");
    _mainContentHolderWidget->installEventFilter(this);
    _mainContentHolderLayout = new QVBoxLayout;
    _mainContentHolderLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    _mainContentHolderWidget->setLayout(_mainContentHolderLayout);

    for(int i = 0; i < sectionCount; ++i)
    {
        this->addSection(this->_delegate->getSectionTitle(i), this->_delegate->getSectionTitleLabelStyleSheet(i), this->_delegate->getSectionWidgetStyleSheet(i));
    }

    this->setWidget(_mainContentHolderWidget);

    _mouseGrabScrollDelegate = new MouseGrabScrollDelegate(400, 2.0f, 5, 0.3f, this);
    _mouseGrabScrollDelegate->setTargetScrollArea(this);
    this->installEventFilter(this);
}

MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::~MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea() {
    qDeleteAll(this->_sections);
}

void MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::resizeEvent(QResizeEvent *e)
{
    QScrollArea::resizeEvent(e);
#if defined(ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING)
    DLog(" --- resize: ") << e->size();
#endif

    int calculatedMaxSectionWidth = 0;
    int verticalScrollBarWidth = this->verticalScrollBar()->width();
    int allowedMaxSectionWidth = this->contentsRect().width() - verticalScrollBarWidth - _mainContentHolderWidget->contentsMargins().left() - _mainContentHolderWidget->contentsMargins().right() - _mainContentHolderLayout->contentsMargins().left() - _mainContentHolderLayout->contentsMargins().right();
//    int allowedMaxSectionWidth = e->size().width() - verticalScrollBarWidth;

    for(int i = 0; i < _sections.size(); ++i)
    {
        SameSizeLayoutBase *sectionLayout = _sections[i]->getSectionLayout();
        QWidget *sectionWidget = _sections[i]->getSectionWidget();

        sectionLayout->setSizeConstraint(QLayout::SetMinimumSize);
        sectionLayout->recalculateMinimalAndOptimalSizesByTotalLayoutWidth(allowedMaxSectionWidth);
        sectionWidget->setMinimumSize(sectionLayout->getCalculatedMinimalSize());
        sectionWidget->resize(sectionLayout->getCalculatedOptimalSize());
//        sectionWidget->setMaximumSize(sectionLayout->getCalculatedOptimalSize());

        calculatedMaxSectionWidth = qMax(calculatedMaxSectionWidth, sectionLayout->getCalculatedOptimalSize().width());
#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
        DLog("max section width: ") << calculatedMaxSectionWidth;
#endif
    }

    //
    // main content holders
    int contentHolderWidgetWidth = _mainContentHolderWidget->contentsMargins().left() + _mainContentHolderLayout->contentsMargins().left() + calculatedMaxSectionWidth + _mainContentHolderLayout->contentsMargins().right() + _mainContentHolderWidget->contentsMargins().right();

#ifdef ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DLog("content holder width: ") << contentHolderWidgetWidth;
#endif

    _mainContentHolderWidget->setFixedWidth(contentHolderWidgetWidth);
    _mainContentHolderWidget->adjustSize(); // needed for vertical adjust
}

void MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::addSection() {
    int newSectionIndex = this->_sections.size();
    this->addSection(_delegate->getSectionTitle(newSectionIndex), _delegate->getSectionTitleLabelStyleSheet(newSectionIndex), _delegate->getSectionWidgetStyleSheet(newSectionIndex));
}

void MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::addSection(QString sectionTitleText, QString sectionTitleLabelStyleSheet, QString sectionWidgetStyleSheet, QString sectionMoreIdentifier)
{
    QWidget *sectionWidget = new QWidget;
    sectionWidget->setStyleSheet(sectionWidgetStyleSheet);

    this->addSectionWithWidget(sectionTitleText, sectionTitleLabelStyleSheet, sectionWidget, sectionMoreIdentifier);
}

void MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::addSectionWithWidget(QString sectionTitleText, QString sectionTitleLabelStyleSheet, QWidget *customSectionContentHolderWidget, QString sectionMoreIdentifier)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    QWidget* titleWidget = new QWidget;
    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0,0,0,0);

    titleWidget->setLayout(titleLayout);
    titleWidget->setStyleSheet(guiConfManager->getStyleForId("Store/SectionWidget/SectionTitle/style"));

    // section title if given

    if(!sectionTitleText.isEmpty()) {
        QLabel *sectionTitleLabel = new QLabel(sectionTitleText);
        sectionTitleLabel->setStyleSheet(sectionTitleLabelStyleSheet);
        titleLayout->addWidget(sectionTitleLabel);
    }

    titleLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    QPushButton* more = 0;
    if (!sectionMoreIdentifier.isEmpty()) {
        more = new QPushButton(tr("Show more"));
        more->setFlat(true);
        more->setStyleSheet(guiConfManager->getStyleForId("Button/Default/ShowMore/style"));

        titleLayout->addWidget(more);
    }

    _mainContentHolderLayout->addWidget(titleWidget);

    SameSizeLayoutBase *sectionLayout = _delegate->createLayout();
    customSectionContentHolderWidget->setLayout(sectionLayout);
    customSectionContentHolderWidget->installEventFilter(this);

    MultiSectionData* sectionData = new MultiSectionData(customSectionContentHolderWidget, sectionLayout, sectionMoreIdentifier);
    if (!sectionMoreIdentifier.isEmpty() &&
        more) {
        connect(more, SIGNAL(clicked()), sectionData, SLOT(sectionActivated()));
        connect(sectionData, SIGNAL(sectionIdentifierRequested(QString)), this, SIGNAL(showListPageWithId(QString)));
    }

    this->_sections.append(sectionData);
    _mainContentHolderLayout->addWidget(customSectionContentHolderWidget);
}

void MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::addWidgetToSection(QWidget *widget, int sectionIndex) {
    if(sectionIndex >= 0 && sectionIndex < _sections.size()) {
        SameSizeLayoutBase *sectionLayout = this->_sections[sectionIndex]->getSectionLayout();
        widget->installEventFilter(this);
        sectionLayout->addWidget(widget);
    }
    else {
        WLog("Invalid section index: ") << sectionIndex;
    }
}


bool MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::processMouseWheelEventFilter(QWheelEvent *wheelEvent)
{
    if (wheelEvent->orientation() == Qt::Horizontal) {
        return false;
    }

#if defined(Q_OS_WIN)
    // emulate the wheel-accelerate-decelerate on Windows (on Mac it's handled by default)
//    this->mouseGrabScrollDelegate->doSmoothDropScroll(0, -1 * wheelEvent->delta() * 2.3f);
    this->_mouseGrabScrollDelegate->accelerateWheelForce(0, wheelEvent->delta());

    return true;
#elif defined(Q_OS_MAC)
    return false;
#endif
}

bool MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::processMouseButtonPressEvent(QObject *obj, QMouseEvent *e)
{
    return this->_mouseGrabScrollDelegate->handleMouseButtonPressEvent(e);
}

bool MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::processMouseMoveEvent(QObject *obj, QMouseEvent *e)
{
    return _mouseGrabScrollDelegate->handleMouseMoveEvent(e);
}

bool MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::processMouseButtonReleaseEvent(QObject *obj, QMouseEvent *e)
{
    return this->_mouseGrabScrollDelegate->handleMouseButtonReleaseEvent(e);
}

bool MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if(me)
        {
            if( processMouseButtonPressEvent(obj, me) ) {
                me->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }
    else if(event->type() == QEvent::MouseMove)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if(me)
        {
            if( processMouseMoveEvent(obj, me) ) {
                me->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }
    else if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if(me)
        {
            bool resultOfMouseButtonReleseDelegation = processMouseButtonReleaseEvent(obj, me);
            if( resultOfMouseButtonReleseDelegation ) {
                me->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }

    else if(event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent *>(event);
        if(wheelEvent) {
            if(this->processMouseWheelEventFilter(wheelEvent)) {
                wheelEvent->accept();
                return true;
            }
        }
        else {
            WLog("Cannot cast");
        }
    }

    return QScrollArea::eventFilter(obj, event);
}
