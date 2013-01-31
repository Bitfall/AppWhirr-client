#include "crossfaderwidget.h"

#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QStyleOption>

CrossFaderWidget::CrossFaderWidget(QWidget *parent) :
    QWidget(parent)
{
    // gui updater
    {
        guiUpdaterTimer = new QTimer(this);
        guiUpdaterTimer->setInterval(40); // about 25 FPS
        connect(guiUpdaterTimer, SIGNAL(timeout()), this, SLOT(update()));
        guiUpdaterTimer->stop();
    }

    this->setAttribute(Qt::WA_NoSystemBackground, true);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);

    // animation
    {
        fadeAnimation = new QPropertyAnimation(this, "transitionPercentage", this);
        fadeAnimation->setStartValue(0.0f);
        fadeAnimation->setEndValue(1.0f);
        fadeAnimation->setEasingCurve(QEasingCurve::InOutExpo);
//        connect(fadeAnimation, SIGNAL(finished()), this, SIGNAL(fadeFinish()));
        connect(fadeAnimation, SIGNAL(finished()), this, SLOT(_fadeAnimationFinished()));
    }
}

CrossFaderWidget::~CrossFaderWidget()
{
    //qDebug("destr");
//    this->stopFading();
}

void CrossFaderWidget::takeAndStoreSnapshotsAboutWidgets(QWidget *sourceWidget, QWidget *targetWidget)
{
    // grab a snapshots of the widgets

    // source
    {
        sourceWidgetSnapshot = QPixmap::grabWidget(sourceWidget);

        // Create new picture for transparent
        QPixmap transparent(sourceWidgetSnapshot.size());

        // Do transparency
        transparent.fill(Qt::transparent);
        QPainter p(&transparent);
//        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawPixmap(0, 0, sourceWidgetSnapshot);

        // Set original picture's reference to new transparent one
        sourceWidgetSnapshot = transparent;
    }

    // target
    {
        targetWidgetSnapshot = QPixmap::grabWidget(targetWidget);

        // Create new picture for transparent
        QPixmap transparent(targetWidgetSnapshot.size());
        //            QPixmap transparent(targetWidget.size());

        // Do transparency
        transparent.fill(Qt::transparent);
        QPainter p(&transparent);
//        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawPixmap(0, 0, targetWidgetSnapshot);

        // Set original picture's reference to new transparent one
        targetWidgetSnapshot = transparent;
    }
}

void CrossFaderWidget::takeSnapshotsOfWidgetsAndFadeBetweenThem(QWidget *sourceWidget, QWidget *targetWidget,
                                                                QRect sourceSnapshotGeometry, QRect targetSnapshotGeometry,
                                                                int durationMillisec)
{
//    Q_EMIT fadeFinish(); // FOR TEST ONLY
    this->takeSnapshotsOfWidgetsAndInitializeFading(sourceWidget, targetWidget, sourceSnapshotGeometry, targetSnapshotGeometry);

    // animation
    fadeAnimation->setDuration(durationMillisec);
    fadeAnimation->start();

    guiUpdaterTimer->start();
//    QTimer::singleShot(50, guiUpdaterTimer, SLOT(start()));
}

void CrossFaderWidget::takeSnapshotsOfWidgetsAndInitializeFading(QWidget *sourceWidget, QWidget *targetWidget,
                                                                 QRect sourceSnapshotGeometry, QRect targetSnapshotGeometry)
{
    sourceGeom = sourceSnapshotGeometry;
    targetGeom = targetSnapshotGeometry;

    this->takeAndStoreSnapshotsAboutWidgets(sourceWidget, targetWidget);
}

void CrossFaderWidget::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html
//    QStyleOption opt;
//    opt.init(this);
    QPainter painter(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);


//    DLog("paint");
//    DLog(" - source: ") << sourceGeom << sourceWidgetSnapshot.isNull();
//    DLog(" - target: ") << targetGeom << targetWidgetSnapshot.isNull();

    painter.setOpacity(1.0f - transitionPercentage);
    painter.drawPixmap(sourceGeom, sourceWidgetSnapshot);

    painter.setOpacity(transitionPercentage);
    painter.drawPixmap(targetGeom, targetWidgetSnapshot);

//    DLog("paint done");
}

void CrossFaderWidget::_fadeAnimationFinished()
{
    guiUpdaterTimer->stop();
    //this->hide();
    //this->deleteLater();

    Q_EMIT fadeFinish();
}

float CrossFaderWidget::getTransitionPercentage() const
{
    return transitionPercentage;
}

void CrossFaderWidget::setTransitionPercentage(float value)
{
    transitionPercentage = value;
}

void CrossFaderWidget::stopFading()
{
    fadeAnimation->stop();
    this->_fadeAnimationFinished();
}
