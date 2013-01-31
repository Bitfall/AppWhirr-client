#include "iconimagewidget.h"

#include <QPainter>
#include <QTransform>
#include <QPicture>
#include <QSvgRenderer>

#include <QStylePainter>

#include "Settings/guiconfigurations.h"

IconImageWidget::IconImageWidget(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum initialState, QWidget *parent) :
    SimpleImagePresenterWidget(parent),
    _isRotationValueChanged(false),
    _rotationValue(0),
    _relatedAppState(initialState)
{
}

void IconImageWidget::paintEvent(QPaintEvent *e)
{
    // TODO: test
//    QWidget::paintEvent(e); // draws the background etc.


    QRect fullRect(0, 0, this->width() - 1, this->height() - 1);

    //this->refreshThePicture();

    //QPainter painter(this);
    QStylePainter painter(this); // TODO: test
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // finally, draw it

    if(!this->_pixmapToPresent.isNull())
    {
        if(_isRotationValueChanged)
        {
            //qDebug(QString("rot p: %1").arg(rotationValue).toAscii());
            QTransform transf;
            painter.setTransform(transf.rotate(40.0f * _rotationValue, Qt::YAxis));
            painter.setTransform(transf.rotate(-20.0f * _rotationValue, Qt::XAxis));
            //painter.setTransform(transf.rotate(45, Qt::YAxis));
            //painter.setTransform(transf.rotate(30, Qt::ZAxis));

            _isRotationValueChanged = false;
        }

        //QTransform transf;
        //painter.setTransform(transf.rotate(45.0f, Qt::YAxis));

        if(_relatedAppState == AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled) {
            painter.setOpacity(0.5f);
        }

        painter.drawPixmap(fullRect, this->_pixmapToPresent);
    } else {
        painter.fillRect(fullRect, QBrush(QColor(100, 100, 100)));
    }


    painter.end();
}

void IconImageWidget::setRelatedAppState(AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newState)
{
    this->_relatedAppState = newState;

    this->forceFullRepaint();
}

AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum IconImageWidget::getRelatedAppState() const
{
    return this->_relatedAppState;
}

//
// properties
void IconImageWidget::setRotationValue(float p)
{
    _rotationValue = p;
    _isRotationValueChanged = true;
    //this->repaint();
    this->update(); // this seems to be better than repaint
}

float IconImageWidget::getRotationValue() const
{
    return _rotationValue;
}

