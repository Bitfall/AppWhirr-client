#include "textfaderlabel.h"
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QPainter>

TextFaderLabel::TextFaderLabel(QWidget *parent) :
    QLabel(parent)
{
    // Setup timer
    timer = new QTimer(this);

    timer->setInterval(5000);    // shot after 5 seconds
    timer->setSingleShot(false); // shot multiple times

    // Start the timer
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(showEventSlot()));
    connect(this, SIGNAL(fireManually()), timer, SIGNAL(timeout()));

    // Setup color
    alpha = 1;

    // State machine
    this->_setupStateMachine();

    // Some debug text
    // ---------------
    this->addTextToShow("Press enter to start the selected program");
    this->addTextToShow("Press ESC to close the launcher");
    this->addTextToShow("bazsi baba 69 - keresre");
    // ---------------
}

TextFaderLabel::~TextFaderLabel() {
    timer->stop();
}

void TextFaderLabel::_setupStateMachine() {
    // Setup animation
    QStateMachine *machine = new QStateMachine(this);
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);

    // Set properties
    state1->assignProperty(this, "alpha", 0);
    state2->assignProperty(this, "alpha", 255);

    // Animation will start
    state1->addTransition(this, SIGNAL(_fadeIn()), state2);
    state2->addTransition(this, SIGNAL(_fadeOut()), state1);

    // Animation did end
    connect(state1, SIGNAL(propertiesAssigned()), this, SLOT(_didFadeOut()));
    connect(state2, SIGNAL(propertiesAssigned()), this, SLOT(_didFadeIn()));

    // Set default animation
    QPropertyAnimation* defaultAnim = new QPropertyAnimation(this, "alpha");
    defaultAnim->setDuration(500);
    defaultAnim->setEasingCurve(QEasingCurve::Linear);
    machine->addDefaultAnimation(defaultAnim);

    // Start machine
    machine->setInitialState(state1);
    machine->start();
}

void TextFaderLabel::addTextToShow(const QString& text) {
    elementsToShow.append(text);

    // Fire if no text visible
    if (this->text().isEmpty()) {
        Q_EMIT this->fireManually();
    }
}

void TextFaderLabel::showEventSlot() {
    Q_EMIT this->_fadeOut();
}

void TextFaderLabel::_didFadeOut() {
    if (!elementsToShow.isEmpty()) {
        this->setText(elementsToShow.first());
        elementsToShow.removeFirst();
    } else {
        this->setText(QString(""));
    }

    Q_EMIT this->_fadeIn();
}

void TextFaderLabel::_didFadeIn() {
}

void TextFaderLabel::paintEvent(QPaintEvent* paintEvent) {
    QPainter painter(this);

    painter.setPen(QColor(0, 0, 0, alpha));
    painter.drawText(0, this->height() - 4, this->text());

    QWidget::paintEvent(paintEvent);
}
