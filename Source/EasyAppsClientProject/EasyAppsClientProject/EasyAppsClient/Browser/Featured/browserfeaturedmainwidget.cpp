#include "browserfeaturedmainwidget.h"

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QState>
#include <QStateMachine>
#include <QPropertyAnimation>

BrowserFeaturedMainWidget::BrowserFeaturedMainWidget(QWidget *parent) :
    QWidget(parent)
{
    imageShifted = 0;

    setFixedSize(600, 200);

    timer = new QTimer(this);
    timer->setInterval(5000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeFeaturedImage()));
    timer->start();
    // TODO: hide eseten invalidate!

    _setupStateMachine();
}

BrowserFeaturedMainWidget::~BrowserFeaturedMainWidget() {
    if (machine) {
        delete machine;
    }
}

void BrowserFeaturedMainWidget::_setupStateMachine() {
    // Setup animation
    machine = new QStateMachine;
    QState* state1 = new QState(machine);
    QState* state2 = new QState(machine);

    // Set properties
    state1->assignProperty(this, "imageShifted", 0);
    state2->assignProperty(this, "imageShifted", 600);

    // Animation will start
    state1->addTransition(this, SIGNAL(_shift()), state2);

    // Animation did end
    connect(state2, SIGNAL(propertiesAssigned()), this, SLOT(_didShift()));

    // Set default animation
    QPropertyAnimation* defaultAnim = new QPropertyAnimation(this, "imageShifted");
    defaultAnim->setDuration(250);
    defaultAnim->setEasingCurve(QEasingCurve::Linear);
    machine->addDefaultAnimation(defaultAnim);

    // Start machine
    machine->setInitialState(state1);
    machine->start();
}

void BrowserFeaturedMainWidget::_didShift() {
    imageShifted = 0;

    delete machine;
    _setupStateMachine();
}

void BrowserFeaturedMainWidget::changeFeaturedImage() {
    Q_EMIT this->_shift();
}

void BrowserFeaturedMainWidget::paintEvent(QPaintEvent* paintEvent) {
    QPainter painter(this);

    // Next image
    if (imageShifted > 0) {
        painter.drawImage(imageShifted - 600, 0, QImage(":/featured_app_1"));
    }

    // Current image
    painter.drawImage(imageShifted, 0, QImage(":/featured_app_1"));

//    QWidget::paintEvent(paintEvent);
}
