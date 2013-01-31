#include "parallelanimatedwidgetreplacer.h"

#include <QWidget>
#include <QBoxLayout>

#include <QPropertyAnimation>
#include <QTimer>

#include "Common/Widgets/crossfaderwidget.h"

#include "Common/Helpers/widgetinlayouthelper.h"
#include "Common/Helpers/qsizehelper.h"

#include <QDebug>


ParallelAnimatedWidgetReplacer::ParallelAnimatedWidgetReplacer(QObject *parent) :
    QObject(parent),
    _tmpContainerLayout(NULL),
    durationMilliseconds(2500)
{
    // gui updater
    {
        guiRefresherTimer = new QTimer(this);
        guiRefresherTimer->setInterval(33); // about 30 FPS
        connect(guiRefresherTimer, SIGNAL(timeout()), this, SLOT(_refreshTransitionWidgets()));
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

ParallelAnimatedWidgetReplacer::~ParallelAnimatedWidgetReplacer()
{
    this->jumpToFinalState();

    // delete the temporary fader widgets
    int numOfWidgetItems = replaceItems.size();
    for(int i = 0; i < numOfWidgetItems; i++) {
        CrossFaderWidget *fw = replaceItems[i].getCrossFaderWidget();
        fw->setParent(0);
        delete fw;
    }
}

void ParallelAnimatedWidgetReplacer::_refreshTransitionWidgets()
{
    int numOfWidgetItems = replaceItems.size();
    for(int i = 0; i < numOfWidgetItems; i++) {
        //qDebug(" & repaint");
        replaceItems[i].getCrossFaderWidget()->repaint();
    }
}

void ParallelAnimatedWidgetReplacer::addReplaceRequest(QWidget *sourceWidget, QWidget *targetWidget, bool isAlsoDeleteSourceWidget)
{
    ParallelAnimatedWidgetReplacerItem replaceItem(sourceWidget, targetWidget,
                                                   QSize(-1, -1), QSize(-1, -1),
                                                   new CrossFaderWidget, isAlsoDeleteSourceWidget);

    replaceItems.append(replaceItem);
#if 0
    _tmpTargetWidget = targetWidget;
    _tmpLayout = layout;

    // calculate the widget sizes
    sourceWidgetSize = sourceWidget->geometry().size();
    // remove sourceWidget
    int indexOfSourceWidgetInLayout = layout->indexOf(sourceWidget);
    if(indexOfSourceWidgetInLayout == -1) {
        qDebug(" ! sourceWidget not found in the layout, cannot replace it.");
        return;
    }
    _tmpLayout->removeWidget(sourceWidget);
    sourceWidget->setParent(0);

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


#endif
}

void ParallelAnimatedWidgetReplacer::startBatchReplace()
{
    // set every item's data
    {
        int numOfWidgetItems = replaceItems.size();
        for(int i = 0; i < numOfWidgetItems; i++) {
            CrossFaderWidget *fw = replaceItems[i].getCrossFaderWidget();

            // remove sourceWidget
            int indexOfSourceWidgetInLayout = this->_tmpContainerLayout->indexOf(replaceItems[i].getSourceWidget());
            if(indexOfSourceWidgetInLayout == -1) {
                qDebug(" ! sourceWidget not found in the layout, cannot replace it.");
                return;
            }

//            replaceItems[i].getSourceWidget()->show();
//            replaceItems[i].getSourceWidget()->updateGeometry();
//            replaceItems[i].getSourceWidget()->adjustSize();
//            this->_tmpContainerLayout->update();
//            this->_tmpContainerLayout->invalidate();
//            this->_tmpContainerLayout->activate();

            this->_tmpContainerLayout->removeWidget(replaceItems[i].getSourceWidget());
            replaceItems[i].getSourceWidget()->setParent(0);

//            replaceItems[i].setSourceWidgetSize( WidgetInLayoutHelper::calculateWidgetSizeInLayout(replaceItems[i].getSourceWidget(), this->_tmpContainerLayout) );

            // calculate the sourceWidget size
//            replaceItems[i].getSourceWidget()->adjustSize();
//            replaceItems[i].getSourceWidget()->updateGeometry();
            replaceItems[i].setSourceWidgetSize( replaceItems[i].getSourceWidget()->geometry().size() );
//            replaceItems[i].setSourceWidgetSize( replaceItems[i].getSourceWidget()->sizeHint() );

            // calculate targetWidget size
//            replaceItems[i].setTargetWidgetSize(WidgetInLayoutHelper::calculateWidgetSizeInLayout(replaceItems[i].getTargetWidget(), this->_tmpContainerLayout));
            replaceItems[i].setTargetWidgetSize(replaceItems[i].getTargetWidget()->sizeHint()); // sizeHint is not always the desired one - but the above one is somethimes also wrong
//            replaceItems[i].setTargetWidgetSize( QSize(50, 13) );


            // set the fader widget
            fw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            fw->setFixedSize(replaceItems[i].getSourceWidgetSize());
//            fw->setMinimumSize(replaceItems[i].getSourceWidgetSize());
//            fw->setMaximumSize(replaceItems[i].getSourceWidgetSize());
            fw->takeSnapshotsOfWidgetsAndInitializeFading(replaceItems[i].getSourceWidget(), replaceItems[i].getTargetWidget(),
                                                          QRect(QPoint(0, 0), replaceItems[i].getSourceWidgetSize()), QRect(QPoint(0, 0), replaceItems[i].getTargetWidgetSize()));

            if(replaceItems[i].getIsAlsoDeleteSourceWidget()) {
                delete replaceItems[i].getSourceWidget();
            }

            // and add the fader widget to the place of the source widget
            this->_tmpContainerLayout->insertWidget(indexOfSourceWidgetInLayout, fw);
            fw->show();
        }
    }

    // and start the replace (with animation)
    theAnimation->setDuration(this->durationMilliseconds);
    theAnimation->start();

    guiRefresherTimer->start();
}

void ParallelAnimatedWidgetReplacer::jumpToFinalState()
{
    theAnimation->stop();
    this->_transitionFinished();
}

void ParallelAnimatedWidgetReplacer::_transitionFinished()
{
    guiRefresherTimer->stop();

    // replace the transition-widget(s) with the target widget(s)
    if(_tmpContainerLayout != NULL) {
#if 0
        // replace the fader widget with the target widget
        WidgetInLayoutHelper::replaceWidgetInLayoutWithWidget(faderWidget, _tmpTargetWidget, _tmpLayout);
//      _tmpLayout->removeWidget(faderWidget);
        faderWidget->setParent(0);
#endif
        int numOfWidgetItems = replaceItems.size();
        for(int i = 0; i < numOfWidgetItems; i++) {
            CrossFaderWidget *fw = replaceItems[i].getCrossFaderWidget();
            WidgetInLayoutHelper::replaceWidgetInLayoutWithWidget(fw, replaceItems[i].getTargetWidget(), _tmpContainerLayout);
            fw->setParent(0);
        }

        _tmpContainerLayout = NULL;
    }

    Q_EMIT replaceFinished();
}

double ParallelAnimatedWidgetReplacer::getTransitionPercentage() const
{
    return this->transitionPercentage;
}

void ParallelAnimatedWidgetReplacer::setTransitionPercentage(double value)
{
    this->transitionPercentage = value;

    int numOfWidgetItems = replaceItems.size();
    for(int i = 0; i < numOfWidgetItems; i++) {
        CrossFaderWidget *fw = replaceItems[i].getCrossFaderWidget();
        fw->setTransitionPercentage(this->transitionPercentage);
        QSize updatedSize = replaceItems[i].getSourceWidgetSize() +
                            QSizeHelper::multiplyQSizeWithNumber(replaceItems[i].getTargetWidgetSize() - replaceItems[i].getSourceWidgetSize(), this->transitionPercentage);
        fw->setFixedSize(updatedSize);
//        fw->setMinimumSize(updatedSize);
//        fw->setMaximumSize(updatedSize);

//        qDebug() << "target size:  " << replaceItems[i].getTargetWidgetSize();
//        qDebug() << "Curr size: " << updatedSize;
    }
}

void ParallelAnimatedWidgetReplacer::setTheContainerLayout(QBoxLayout *layout) {
    _tmpContainerLayout = layout;
}

void ParallelAnimatedWidgetReplacer::setDuration(int durationMilliseconds) {
    this->durationMilliseconds = durationMilliseconds;
}
