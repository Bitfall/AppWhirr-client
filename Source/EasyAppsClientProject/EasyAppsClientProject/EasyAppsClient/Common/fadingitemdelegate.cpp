#include "fadingitemdelegate.h"

#include <QStateMachine>
#include <QPropertyAnimation>

#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QTimer>
#include <QWidget>

FadingItemDelegate::FadingItemDelegate(FadingPerformerDelegateInterface *fadingPreformerDelegate, int fadeDurationMilliseconds, QObject *parent) :
    QObject(parent),
    _fadeOpacity(0.0f),
    _isFading(false),
    _isSavingSnapshot(false),
    _currentFadeMode(FM_Out),
    _fadingPerformerDelegate(fadingPreformerDelegate)
{
    // gui updater
    {
        _guiUpdaterTimer = new QTimer(this);
        _guiUpdaterTimer->setInterval(40); // about 25 FPS
        connect(_guiUpdaterTimer, SIGNAL(timeout()), this, SLOT(_updateGui()));
        _guiUpdaterTimer->stop();
    }

    // grow / shrink
    QStateMachine *machine = new QStateMachine();
    QState *state1 = new QState(machine);
    QState *state2 = new QState(machine);

    state1->assignProperty(this, "fadeOpacity", 0.0f);
    state2->assignProperty(this, "fadeOpacity", 1.0f);

    state1->addTransition(this, SIGNAL(__startFadeIn()), state2);
    state2->addTransition(this, SIGNAL(__startFadeOut()), state1);


    // and add a default animation
    QPropertyAnimation *defaultAnim = new QPropertyAnimation(this, "fadeOpacity");
    defaultAnim->setDuration(fadeDurationMilliseconds);
    defaultAnim->setEasingCurve(QEasingCurve::Linear); // QEasingCurve::OutExpo
    machine->addDefaultAnimation(defaultAnim);

    connect(defaultAnim, SIGNAL(finished()), this, SLOT(_fadeAnimationFinished()));
    machine->setInitialState(state1);
    machine->start();
}

void FadingItemDelegate::delegatePaintEventHere(QPaintEvent *e)
{
    if(_isSavingSnapshot) {
        if(this->_fadingPerformerDelegate == NULL) {
            WLog("No fading preformer delegate supported. Required.");
            return;
        }
        this->_fadingPerformerDelegate->performNormalPaint(e);
        this->_isSavingSnapshot = false;
        return;
    }

    if(this->_isFading) {
//        DLog("Paint - fading");
//        DLog("--");

        if(this->_fadingPerformerDelegate == NULL) {
            WLog("No fading preformer delegate supported. Required.");
            return;
        }
        this->_fadingPerformerDelegate->performPaintOnlyThisPixmapWithOpacity(this->_savedSnapshotPixmap, this->_fadeOpacity, e);
    }
    else {
        if(_currentFadeMode == FM_Out) {
            // don't render anything -> fade is done, faded out successfully
        }
        else {
            // not fading - simply render
//            DLog("Paint - normal");

            if(this->_fadingPerformerDelegate == NULL) {
                WLog("No fading preformer delegate supported. Required.");
                return;
            }
            this->_fadingPerformerDelegate->performNormalPaint(e);
        }
    }
}

void FadingItemDelegate::_updateGui()
{
    if(this->_fadingPerformerDelegate == NULL) {
        WLog("No fading preformer delegate supported. Required.");
        return;
    }
    this->_fadingPerformerDelegate->getPresenterWidget()->update();
}

void FadingItemDelegate::saveCurrentSnapshot()
{
    if(this->_fadingPerformerDelegate == NULL) {
        WLog("No fading performer delegate set. Required.");
        return;
    }
    this->_isSavingSnapshot = true;
    this->_savedSnapshotPixmap = QPixmap::grabWidget(this->_fadingPerformerDelegate->getPresenterWidget());
}

void FadingItemDelegate::_startGuiUpdater() {
//    DLog("Update start");
    this->_guiUpdaterTimer->start();
}

void FadingItemDelegate::_stopGuiUpdater() {
//    DLog("Update stop");
    this->_guiUpdaterTimer->stop();
}

// slot
void FadingItemDelegate::_fadeAnimationStarted()
{
    this->saveCurrentSnapshot();
    this->_startGuiUpdater();
    _isFading = true;
}

// slot
void FadingItemDelegate::_fadeAnimationFinished()
{
    _isFading = false;
    this->_stopGuiUpdater();
    // force update again to finalize
    this->_updateGui();

    if(this->_currentFadeMode == FM_In) {
        this->_fadingPerformerDelegate->dynamicSizeAllowed();
        this->_fadingPerformerDelegate->showContentForFadeAnimation();
    }
}

void FadingItemDelegate::startFadeInAnimation() {
//    DLog("Fade in");
    _currentFadeMode = FM_In;
    this->_fadingPerformerDelegate->showContentForFadeAnimation();
    this->_fadeAnimationStarted();
    // preserve it's current size
    this->_fadingPerformerDelegate->preserveCurrentSize();
    this->_fadingPerformerDelegate->hideContentForFadeAnimation();
    Q_EMIT __startFadeIn();
}

void FadingItemDelegate::startFadeOutAnimation() {
//    DLog("Fade out");
    _currentFadeMode = FM_Out;
    this->_fadingPerformerDelegate->showContentForFadeAnimation();
    this->_fadeAnimationStarted();
    this->_fadingPerformerDelegate->hideContentForFadeAnimation();
    Q_EMIT __startFadeOut();
}

double FadingItemDelegate::getFadeOpacity() const
{
    return this->_fadeOpacity;
}

void FadingItemDelegate::setFadeOpacity(double value)
{
    this->_fadeOpacity = value;
}

