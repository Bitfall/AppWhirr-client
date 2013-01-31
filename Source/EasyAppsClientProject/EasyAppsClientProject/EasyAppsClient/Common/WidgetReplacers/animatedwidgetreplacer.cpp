#include "animatedwidgetreplacer.h"

#include <QWidget>
#include <QBoxLayout>

#include <QPropertyAnimation>
#include <QTimer>

#include "Common/Widgets/crossfaderwidget.h"

#include "Common/Helpers/widgetinlayouthelper.h"
#include "Common/Helpers/qsizehelper.h"

AnimatedWidgetReplacer::AnimatedWidgetReplacer(QObject *parent) :
    QObject(parent),
    _tmpTargetWidget(NULL),
    _tmpLayout(NULL)
{
    faderWidget = new CrossFaderWidget;

    // gui updater
    {
        guiRefresherTimer = new QTimer(this);
        guiRefresherTimer->setInterval(33); // about 30 FPS
        connect(guiRefresherTimer, SIGNAL(timeout()), faderWidget, SLOT(update()));
        guiRefresherTimer->stop();
    }

    // animation
    {
        theAnimation = new QPropertyAnimation(this, "transitionPercentage", this);
        theAnimation->setStartValue(0.0f);
        theAnimation->setEndValue(1.0f);
        theAnimation->setEasingCurve(QEasingCurve::InOutExpo);
        connect(theAnimation, SIGNAL(finished()), this, SLOT(_transitionFinished()));
    }
}

AnimatedWidgetReplacer::~AnimatedWidgetReplacer()
{
    this->jumpToFinalState();

    faderWidget->setParent(0);
    delete faderWidget;
}

void AnimatedWidgetReplacer::replaceWidgetInLayoutToWidget(QWidget *sourceWidget, QWidget *targetWidget, QBoxLayout *layout,
                                                           int durationMilliseconds, bool isAlsoDeleteSourceWidget)
{
    _tmpTargetWidget = targetWidget;
    _tmpLayout = layout;

    // remove sourceWidget
    int indexOfSourceWidgetInLayout = layout->indexOf(sourceWidget);
    if(indexOfSourceWidgetInLayout == -1) {
        qDebug(" ! sourceWidget not found in the layout, cannot replace it.");
        return;
    }
    _tmpLayout->removeWidget(sourceWidget);
    sourceWidget->setParent(0);

    // calculate the sourceWidget size
    sourceWidgetSize = sourceWidget->geometry().size();

    // calculate targetWidget size
    targetWidgetSize = WidgetInLayoutHelper::calculateWidgetSizeInLayout(targetWidget, _tmpLayout);


    // set the fader widget
    faderWidget->setFixedSize(sourceWidgetSize);
    faderWidget->takeSnapshotsOfWidgetsAndInitializeFading(sourceWidget, targetWidget,
                                                           QRect(QPoint(0, 0), sourceWidgetSize), QRect(QPoint(0, 0), targetWidgetSize));

    if(isAlsoDeleteSourceWidget) {
        delete sourceWidget;
    }

    // and add the fader widget to the place of the source widget
    _tmpLayout->insertWidget(indexOfSourceWidgetInLayout, faderWidget);

    // start the replace animation
    theAnimation->setDuration(durationMilliseconds);
    theAnimation->start();
}

void AnimatedWidgetReplacer::jumpToFinalState()
{
    theAnimation->stop();
    this->_transitionFinished();
}

void AnimatedWidgetReplacer::_transitionFinished()
{
    guiRefresherTimer->stop();

    if(_tmpTargetWidget != NULL && _tmpLayout != NULL) {
        // replace the fader widget with the target widget
        WidgetInLayoutHelper::replaceWidgetInLayoutWithWidget(faderWidget, _tmpTargetWidget, _tmpLayout);
//      _tmpLayout->removeWidget(faderWidget);
        faderWidget->setParent(0);

        _tmpTargetWidget = NULL;
        _tmpLayout = NULL;
    }

    Q_EMIT replaceFinished();
}

float AnimatedWidgetReplacer::getTransitionPercentage() const
{
    return transitionPercentage;
}

void AnimatedWidgetReplacer::setTransitionPercentage(float value)
{
    transitionPercentage = value;

    faderWidget->setTransitionPercentage(transitionPercentage);
    faderWidget->setFixedSize(sourceWidgetSize + QSizeHelper::multiplyQSizeWithNumber(targetWidgetSize - sourceWidgetSize, value));
}
