#include "widgetanimatedtwosizeresizer.h"

// animation
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QTimer>

//
#include <QStyleOption>
#include <QPaintEvent>
#include <QPainter>

// utility
#include <QDebug>
#include "Common/Helpers/qsizehelper.h"


WidgetAnimatedTwoSizeResizer::WidgetAnimatedTwoSizeResizer(QWidget *targetWidget, int sizeChangeDurationMilliseconds, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, bool isSetWidth, bool isSetHeight, WidgetResizeModeEnum widgetResizeMode, QObject *parent) :
    QObject(parent),
    _targetWidget(targetWidget),
    _isExpanded(isExpandedByDefault),
    _currFixedSize(QSize(0,0)),
//    _isSetOnlyMinimumSize(false),
    _widgetResizeMode(widgetResizeMode),
    _isSetWidth(isSetWidth),
    _isSetHeight(isSetHeight),
    _sizeChangeDurationMilliseconds(sizeChangeDurationMilliseconds)
{
    if(isExpandedByDefault) {
        this->_currFixedSize = expandedSize;
    } else {
        this->_currFixedSize = collapsedSize;
    }

    // state machine

    // expand / collapse
    QStateMachine *machine = new QStateMachine();
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);

    state1->assignProperty(this, "currFixedSize", collapsedSize);
    state2->assignProperty(this, "currFixedSize", expandedSize);

    state1->addTransition(this, SIGNAL(_switchToExpandedStateSignal()), state2);
    state2->addTransition(this, SIGNAL(_switchToCollapsedStateSignal()), state1);


    // and add a default animation
    QPropertyAnimation *defaultAnim = new QPropertyAnimation(this, "currFixedSize");
    defaultAnim->setDuration(sizeChangeDurationMilliseconds);
    defaultAnim->setEasingCurve(QEasingCurve::OutExpo);
    connect(defaultAnim, SIGNAL(finished()), this, SLOT(stateChangeFinished()));
    machine->addDefaultAnimation(defaultAnim);

    if(isExpandedByDefault) {
        machine->setInitialState(state2);
    } else {
        machine->setInitialState(state1);
    }
    machine->start();

    if(!isExpandedByDefault) {
        if(collapsedSize.width() < 5 || collapsedSize.height() < 5) {
            _targetWidget->hide();
        }
    }

    _guiRefreshTimer = new QTimer(this);
    _guiRefreshTimer->setSingleShot(false);
    _guiRefreshTimer->setInterval(33); // 30 FPS
    _guiRefreshTimer->stop();
    connect(_guiRefreshTimer, SIGNAL(timeout()), this, SLOT(_updateByCurrFixedSize()));

    this->setWidgetResizeMode(_widgetResizeMode);
    this->_updateByCurrFixedSize();
}

//void WidgetAnimatedTwoSizeResizer::_setIsSetOnlyMinimumSize(bool value)
//{
//    this->_isSetOnlyMinimumSize = value;
//}

void WidgetAnimatedTwoSizeResizer::_setIsSetWidth(bool value) {
    this->_isSetWidth = value;
}

void WidgetAnimatedTwoSizeResizer::_setIsSetHeight(bool value) {
    this->_isSetHeight = value;
}

void WidgetAnimatedTwoSizeResizer::stateChangeFinished()
{
    _guiRefreshTimer->stop();
    this->_updateByCurrFixedSize();

    if(this->getCurrFixedSize().width() < 5 || this->getCurrFixedSize().height() < 5) {
        // the widget is so small that we can hide it
        _targetWidget->hide();
    }

    if(this->_isExpanded) {
//        this->_switchToExpandedStateFinished();
        Q_EMIT switchToExpandedStateFinished();
    }
    else {
//        this->_switchToCollapsedStateFinished();
        Q_EMIT switchToCollapsedStateFinished();
    }
}

//QSize WidgetAnimatedTwoSizeResizer::sizeHint() const
//{
//    QSizeF s = this->getCurrFixedSize();
//    QSize intBasedSize = QSizeHelper::normalizedIntBasedSizeFromFloatBasedSize(s);
////    qDebug() << "sizeHint: " << intBasedSize;
//    return intBasedSize;
//}

QSizeF WidgetAnimatedTwoSizeResizer::getCurrFixedSize() const
{
//    qDebug() << "Get FSize: " << this->_currFixedSize;
    return this->_currFixedSize;
}

void WidgetAnimatedTwoSizeResizer::setCurrFixedSize(QSizeF newFixedSize)
{
//    qDebug() << "FSize: " << newFixedSize;
    this->_currFixedSize = newFixedSize;


    //    this->updateGeometry();
}

void WidgetAnimatedTwoSizeResizer::setWidgetResizeMode(WidgetAnimatedTwoSizeResizer::WidgetResizeModeEnum widgetResizeMode)
{
    _widgetResizeMode = widgetResizeMode;

    if(_widgetResizeMode == WRM_SetBothMinimumAndMaximumSizes) {
        if(_isSetWidth && _isSetHeight) {
            _targetWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        }
        else if(_isSetWidth) {
            _targetWidget->setSizePolicy(QSizePolicy::Fixed, _targetWidget->sizePolicy().verticalPolicy());
        }
        else if(_isSetHeight) {
            _targetWidget->setSizePolicy(_targetWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
        }
//        else {
//            _targetWidget->setSizePolicy(_targetWidget->sizePolicy());
//        }
    }
    else if(_widgetResizeMode == WRM_SetOnlyMinimumSize) {
        if(_isSetWidth && _isSetHeight) {
            _targetWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        }
        else if(_isSetWidth) {
            _targetWidget->setSizePolicy(QSizePolicy::Minimum, _targetWidget->sizePolicy().verticalPolicy());
        }
        else if(_isSetHeight) {
            _targetWidget->setSizePolicy(_targetWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
        }
    }
    else if(_widgetResizeMode == WRM_SetOnlyMaximumSize) {
        if(_isSetWidth && _isSetHeight) {
            _targetWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        }
        else if(_isSetWidth) {
            _targetWidget->setSizePolicy(QSizePolicy::Maximum, _targetWidget->sizePolicy().verticalPolicy());
        }
        else if(_isSetHeight) {
            _targetWidget->setSizePolicy(_targetWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
        }
    }
}

// slot
void WidgetAnimatedTwoSizeResizer::_updateByCurrFixedSize()
{
    QSize currSizeInt = QSizeHelper::normalizedIntBasedSizeFromFloatBasedSize(this->_currFixedSize);

    if(_widgetResizeMode == WRM_SetBothMinimumAndMaximumSizes)
    {
        if(_isSetWidth) {
            _targetWidget->setMinimumWidth(currSizeInt.width());
            _targetWidget->setMaximumWidth(currSizeInt.width());
        }
        if(_isSetHeight) {
            _targetWidget->setMinimumHeight(currSizeInt.height());
            _targetWidget->setMaximumHeight(currSizeInt.height());
        }
    }
    else if(_widgetResizeMode == WRM_SetOnlyMinimumSize)
    {
        if(_isSetWidth) {
            _targetWidget->setMinimumWidth(currSizeInt.width());
        }
        if(_isSetHeight) {
            _targetWidget->setMinimumHeight(currSizeInt.height());
        }
    }
    else if(_widgetResizeMode == WRM_SetOnlyMaximumSize)
    {
        if(_isSetWidth) {
            _targetWidget->setMaximumWidth(currSizeInt.width());
        }
        if(_isSetHeight) {
            _targetWidget->setMaximumHeight(currSizeInt.height());
        }
    }
}

// slot
void WidgetAnimatedTwoSizeResizer::switchToExpandedStateSlot()
{
    if(this->_isExpanded) {
        // already expanded
        return;
    }

    _targetWidget->show();
    this->_isExpanded = true;
    Q_EMIT _switchToExpandedStateSignal();

    _guiRefreshTimer->start();
    this->_updateByCurrFixedSize();
}

// slot
void WidgetAnimatedTwoSizeResizer::switchToCollapsedStateSlot()
{
    if(!this->_isExpanded) {
        // already collapsed
        return;
    }
    this->_isExpanded = false;
    Q_EMIT _switchToCollapsedStateSignal();

    _guiRefreshTimer->start();
    this->_updateByCurrFixedSize();
}

void WidgetAnimatedTwoSizeResizer::switchState(bool isExpanded)
{
    if(isExpanded) {
        this->switchToExpandedStateSlot();
    }
    else {
        this->switchToCollapsedStateSlot();
    }
}

bool WidgetAnimatedTwoSizeResizer::getIsExpanded() const
{
    return this->_isExpanded;
}
