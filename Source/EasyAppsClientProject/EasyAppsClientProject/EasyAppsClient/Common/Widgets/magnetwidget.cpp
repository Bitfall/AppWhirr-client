#include "magnetwidget.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>

#include <QTimer>

MagnetWidget::MagnetWidget(QWidget *parent) :
    QWidget(parent),
    _magnetTargetWidget(NULL)
{
    QTimer *forceUpdateTimer = new QTimer(this);
    forceUpdateTimer->setInterval(100);
    connect(forceUpdateTimer, SIGNAL(timeout()), this, SLOT(updateMagnetTargetGeometry()));

    forceUpdateTimer->start();
}

void MagnetWidget::setMagnetTargetWidget(QWidget *targetWidget, bool isApplyEventFilter, bool isApplyFlagsForFramelessWindow, bool isReparent)
{
    _magnetTargetWidget = targetWidget;

    if(isReparent) {
        _magnetTargetWidget->setParent(this);
    }

    if(isApplyFlagsForFramelessWindow) {
        _magnetTargetWidget->setAttribute(Qt::WA_TranslucentBackground, false);
        _magnetTargetWidget->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    }

    if(isApplyEventFilter) {
        _magnetTargetWidget->installEventFilter(this);
    }

    _magnetTargetWidget->hide();
}

QWidget *MagnetWidget::getMagnetTargetWidget() const
{
    return _magnetTargetWidget;
}

void MagnetWidget::updateMagnetTargetGeometry()
{
//    DLog("Visible region: ") << this->visibleRegion();
    this->refreshTargetWidgetVisibility();

//    if(_magnetTargetWidget != NULL) {
////        QRect globalWebViewRect = QRect(this->mapToGlobal(QPoint(0, 0)), this->geometry().size());
//        QRect globalWebViewRect = QRect(this->mapToGlobal(QPoint(0, 0)), _magnetTargetWidget->geometry().size());
//        _magnetTargetWidget->setGeometry(globalWebViewRect);
//        DLog("magnet target geom update");
//    }

//    this->repaint();
    this->update();
}

void MagnetWidget::resizeEvent(QResizeEvent *e)
{
    this->updateMagnetTargetGeometry();

    QWidget::resizeEvent(e);
}

bool MagnetWidget::event(QEvent *e)
{
    if(_magnetTargetWidget == NULL) {
        return QWidget::event(e);
    }


    if(e->type() == QEvent::Move ||
            e->type() == QEvent::Paint)
    {
        this->updateMagnetTargetGeometry();
    }
    else if(e->type() == QEvent::Hide) {
        this->hideTargetWidget();
    }
    else if(e->type() == QEvent::Show) {
//            this->showTargetWidget();
        _isShowTargetWidgetAtNextPaintEvent = true;
    }
//    else if(e->type() == QEvent::UpdateLater) {
//        DLog("Update request");
//        this->hideTargetWidgetIfInvisible();
//    }

    return QWidget::event(e);
}

bool MagnetWidget::eventFilter(QObject *o, QEvent *e)
{
    if(_magnetTargetWidget != NULL && o == _magnetTargetWidget) {
        if(e->type() == QEvent::Close) {
            DLog("magnet target widget - close");
            // don't allow it!
            e->accept();
            return true;
        }
    }

    QWidget::eventFilter(o, e);
}

void MagnetWidget::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    if(_magnetTargetWidget != NULL) {
        QSize targetSize = e->region().boundingRect().size();
        QPoint p_t = this->mapToGlobal(QPoint(0, 0));
        QRect globalWebViewRect = QRect(QPoint(p_t.x() + e->region().boundingRect().x(), p_t.y() + e->region().boundingRect().y()), targetSize);
        _magnetTargetWidget->setGeometry(globalWebViewRect);

//        if(globalWebViewRect.width() < 5 || globalWebViewRect.height() < 5) {
//            this->hideTargetWidget();
//        }

//        DLog("Geom, region: ") << e->region().boundingRect() << this->geometry() << this->contentsRect();
//        DLog("magnet target geom update");
    }

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if(_isShowTargetWidgetAtNextPaintEvent) {
        _isShowTargetWidgetAtNextPaintEvent = false;
        this->showTargetWidget();
    }
}

void MagnetWidget::showTargetWidget()
{
    DLog("-- magnet widget: SHOW");
    _magnetTargetWidget->show();
    _magnetTargetWidget->raise();
    _magnetTargetWidget->repaint();
    _magnetTargetWidget->update();
}

void MagnetWidget::hideTargetWidget()
{
    DLog("-- magnet widget: HIDE");
    _magnetTargetWidget->hide();
}

void MagnetWidget::refreshTargetWidgetVisibility()
{
    if(this->visibleRegion().isEmpty() && !_magnetTargetWidget->isHidden()) {
        DLog("Empty visible region.");
        this->hideTargetWidget();
    }

    else if(!this->visibleRegion().isEmpty() && _magnetTargetWidget->isHidden()) {
        this->showTargetWidget();
    }
}
