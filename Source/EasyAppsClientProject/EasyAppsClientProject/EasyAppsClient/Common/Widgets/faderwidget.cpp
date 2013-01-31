#include "faderwidget.h"

#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>


FaderWidget::FaderWidget(int fadeDuration, QWidget *parent) :
    QWidget(parent),
    _fadeDuration(fadeDuration)
{
    fadeRefreshTimer = new QTimer(this);
    fadeRefreshTimer->setInterval(33);
    connect(fadeRefreshTimer, SIGNAL(timeout()), this, SLOT(update()));
    fadeRefreshTimer->stop();

    this->setAttribute(Qt::WA_NoSystemBackground, true);
}

FaderWidget::~FaderWidget()
{
    //qDebug("destr");
}

void FaderWidget::_doFade(FadeDirection fadeDirection, float startValue, float endValue, int duration, QWidget *targetWidget)
{
//    this->setGeometry(targetWidget->geometry());
    this->setFixedSize(targetWidget->size());

    // grab a snapshot of the widget
    {
#if 0
        targetWidgetSnapshot = QPixmap(targetWidget->rect().size());
        targetWidgetSnapshot.fill(Qt::transparent);
        targetWidgetSnapshot = QPixmap::grabWidget(targetWidget);
        //targetWidgetSnapshot.grabWidget(targetWidget);
#endif
        targetWidgetSnapshot = QPixmap::grabWidget(targetWidget);
        //this->setMinimumSize(targetWidget->minimumSize());
        //this->setGeometry(targetWidget->geometry());
        /*QRect tmpGeom = QRect(targetWidget->pos().x(), targetWidget->pos().y(), targetWidget->geometry().width(), targetWidget->geometry().height());
        this->setGeometry(tmpGeom);*/



    #if 0
        transpImage = targetWidgetSnapshot.toImage();
        QPainter transparentPainter(&transpImage);
        {
            /*transparentPainter.setCompositionMode(QPainter::CompositionMode_Source);
            transparentPainter.drawPixmap(0, 0, targetWidgetSnapshot);*/
            transparentPainter.setCompositionMode(QPainter::CompositionMode_Multiply);
            transparentPainter.fillRect(targetWidgetSnapshot.rect(), QColor(255, 255, 255, 50));
        }
        transparentPainter.end();
    #endif

        // Create new picture for transparent
        QPixmap transparent(targetWidgetSnapshot.size());
        //QImage transparent(targetWidgetSnapshot.size(), QImage::Format_ARGB32_Premultiplied);

        // Do transparency
        transparent.fill(Qt::transparent);
        QPainter p(&transparent);
//        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawPixmap(0, 0, targetWidgetSnapshot);
        //p.fillRect(transparent.rect(), QColor(0, 0, 0, 255));
        p.end();

        // Set original picture's reference to new transparent one
        //targetWidgetSnapshot = QPixmap::fromImage(transparent);
        targetWidgetSnapshot = transparent;
    }

    // animation
    {
        QParallelAnimationGroup *animGroup = new QParallelAnimationGroup;

        // fade
        {
            QPropertyAnimation *anim = new QPropertyAnimation(this, "fadeOpacity");
            anim->setDuration(duration);
            anim->setStartValue(startValue);
            anim->setEndValue(endValue);
            anim->setEasingCurve(QEasingCurve::InOutExpo);
            connect(anim, SIGNAL(finished()), this, SLOT(_fadeAnimationFinished()));
            //anim->start(QAbstractAnimation::DeleteWhenStopped);
            animGroup->addAnimation(anim);
        }

//        // bit of movement
//        {
//            QRect contGeom = targetWidget->geometry();
//            //QSize tmpSize = (contGeom.size() - this->rect().size()) / 2;
//            //QSize tmpSize = QSize(contGeom.x(), contGeom.y());
//            QPoint endAnimPoint;
//            QPoint startAnimPoint;
//            if(fadeDirection == FD_In) {
//                startAnimPoint = QPoint(contGeom.x(), contGeom.y() - 30);
//                endAnimPoint = QPoint(contGeom.x(), contGeom.y());
//            } else {
//                startAnimPoint = QPoint(contGeom.x(), contGeom.y());
//                endAnimPoint = QPoint(contGeom.x(), contGeom.y() - 30);
//            }
//            QPropertyAnimation *showMoveAnim = new QPropertyAnimation(this, "geometry");
//            showMoveAnim->setDuration(400);
//            showMoveAnim->setStartValue(QRect(startAnimPoint, this->size()));
//            showMoveAnim->setEndValue(QRect(endAnimPoint, this->size()));
//            showMoveAnim->setEasingCurve(QEasingCurve::InOutQuad);

//            //showMoveAnim->start(QAbstractAnimation::DeleteWhenStopped);
//            animGroup->addAnimation(showMoveAnim);
//        }

        animGroup->start(QAbstractAnimation::DeleteWhenStopped);
    }

    fadeRefreshTimer->start();

    this->raise();
}

void FaderWidget::fadeThisWidgetOut(QWidget *targetWidget)
{
    this->_doFade(FD_Out, 1, 0, _fadeDuration, targetWidget);
}

void FaderWidget::fadeThisWidgetIn(QWidget *targetWidget)
{
    // grab a snapshot of the widget
    this->_doFade(FD_In, 0, 1, _fadeDuration, targetWidget);
}

void FaderWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setOpacity(fadeOpacity);
    painter.drawPixmap(this->rect(), targetWidgetSnapshot);
    //painter.fillRect(this->rect(), QColor(150, 200, 200));
}

void FaderWidget::_fadeAnimationFinished()
{
    fadeRefreshTimer->stop();
//    this->hide();
//    this->deleteLater();

    Q_EMIT fadeFinish();
}

float FaderWidget::getFadeOpacity() const
{
    return fadeOpacity;
}

void FaderWidget::setFadeOpacity(float value)
{
    fadeOpacity = value;
}
