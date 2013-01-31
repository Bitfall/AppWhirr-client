#include "layeredimagepresenterwidget.h"

#include <QPainter>
#include <QMouseEvent>

#include <math.h>

LayeredImagePresenterWidget::LayeredImagePresenterWidget(QWidget *parent) :
    QWidget(parent),
    _isAutoResizeToImageSize(false),
    _isFullRepaintNeeded(false)
{
    this->setAttribute(Qt::WA_NoSystemBackground, true);
}

void LayeredImagePresenterWidget::resizeEvent(QResizeEvent *e) {
    this->_isFullRepaintNeeded = true;

    QWidget::resizeEvent(e);
}

void LayeredImagePresenterWidget::paintEvent(QPaintEvent *e)
{
    if(_isFullRepaintNeeded) {
        this->_refreshRenderPixmap();
        this->_isFullRepaintNeeded = false;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(this->rect(), _pixmapToPresent);
}

bool LayeredImagePresenterWidget::event(QEvent *e)
{
    if(e->type() == QEvent::MouseButtonRelease)
    {
        Q_EMIT clicked();
        e->accept();
        return true;
    }

    return QWidget::event(e);
}

void LayeredImagePresenterWidget::addImageLayer(ImageLayerModel imageLayer, bool isForceUpdateAfterAdd)
{
    _imageLayers.append(imageLayer);

    if(this->_isAutoResizeToImageSize) {
        this->resizeToBaseImageLayerSize();
    }

    if(isForceUpdateAfterAdd) {
//      this->update();
        this->forceFullRepaint();
    }
}

void LayeredImagePresenterWidget::setImageLayer(int layerIndex, ImageLayerModel imageLayer)
{
    if( layerIndex > (_imageLayers.size()-1) || layerIndex < 0 ) {
        WLog("Invalid index");
        return;
    }

    _imageLayers[layerIndex] = imageLayer;

    if(this->_isAutoResizeToImageSize) {
        this->resizeToBaseImageLayerSize();
    }

//    if(isForceUpdateAfterAdd) {
//      this->update();
        this->forceFullRepaint();
    //    }
}

QPixmap LayeredImagePresenterWidget::getPresentedImageAsPixmap() const
{
    return _pixmapToPresent;
}

void LayeredImagePresenterWidget::setIsAutoResizeToBaseImageLayerSize(bool isAutoResize)
{
    _isAutoResizeToImageSize = isAutoResize;
    if(_isAutoResizeToImageSize) {
        this->resizeToBaseImageLayerSize();
    }

    this->update();
}

void LayeredImagePresenterWidget::resizeToBaseImageLayerSize() {
//    DLog(" -- Resize to image size");
    if(!_imageLayers.isEmpty()) {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->setFixedSize(_imageLayers[0].getImage().size());
        this->updateGeometry();
    }
}

void LayeredImagePresenterWidget::forceFullRepaint() {
    this->flagFullRepaintNeeded();
    this->repaint();
}

void LayeredImagePresenterWidget::flagFullRepaintNeeded() {
    this->_isFullRepaintNeeded = true;
}

bool LayeredImagePresenterWidget::getIsFullRepaintNeeded() const {
    return this->_isFullRepaintNeeded;
}

void LayeredImagePresenterWidget::_refreshRenderPixmap()
{
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("ImageLayerAlignment is not implemented!");
    if(_imageLayers.isEmpty()) {
        return;
    }

    _pixmapToPresent = QPixmap::fromImage(_imageLayers[0].getImage());

    if(_imageLayers.size() == 0) {
        // there's no additional layer
        return;
    }

    QSize presenterPixmapSize = _pixmapToPresent.size();
    if(presenterPixmapSize.width() <= 0 || presenterPixmapSize.height() <= 0) {
        WLog("Invalid full image size: ") << presenterPixmapSize;
        return;
    }

    QPainter picturePainter( &_pixmapToPresent );
    for(int i = 1; i < _imageLayers.size(); ++i)
    {
        DLog("-paint layer");
        ImageLayerModel currImageLayer = _imageLayers[i];
        QImage currImageLayerImage = currImageLayer.getImage();
        QSize currImageLayerImageSize = currImageLayerImage.size();
        if(currImageLayerImageSize.width() <= 0 || currImageLayerImageSize.height() <= 0) {
            WLog("Invalid layer size: ") << currImageLayerImageSize;
            continue;
        }

//        QRect targetRect = QRect(0, 0, currImageLayerImageSize.width(), currImageLayerImageSize.height());

        // #1: calculate the size of the layer-image
        QSize desiredLayerImageSize = currImageLayerImageSize;
        if(currImageLayer.getResizeMode() == ImageLayerResizeMode::FillTheSpace) {
            // resize mode: fill the space
//            targetRect = QRect(0, 0, presenterPixmapSize.width(), presenterPixmapSize.height());
            desiredLayerImageSize = presenterPixmapSize;
        }
        else if(currImageLayer.getResizeMode() == ImageLayerResizeMode::FitToSpace) {
            // resize mode: fit to space
            if(presenterPixmapSize.width() > 0 && currImageLayerImageSize.width() > 0 &&
                    presenterPixmapSize.height() > 0 && currImageLayerImageSize.height() > 0) {
                float xRatio = (float)presenterPixmapSize.width() / (float)currImageLayerImageSize.width();
                float yRatio = (float)presenterPixmapSize.height() / (float)currImageLayerImageSize.height();

                int calculatedImageW = -1;
                int calculatedImageH = -1;
                if(xRatio > yRatio) {
                    calculatedImageW = floor( (float)currImageLayerImageSize.width() * yRatio );
                    calculatedImageH = floor( (float)currImageLayerImageSize.height() * yRatio );
                }
                else {
                    calculatedImageW = floor( (float)currImageLayerImageSize.width() * xRatio );
                    calculatedImageH = floor( (float)currImageLayerImageSize.height() * xRatio );
                }

                desiredLayerImageSize = QSize(calculatedImageW, calculatedImageH);
            }
            else {
                WLog("Invalid image sizes.");
            }
        }
//        else {
//            // no resize mode
//            if(currImageLayer.getAlignment() == ImageLayerAlignment::Center) {
//                int calcPosX = presenterPixmapSize.width();
//                if(calcPosX != 0) { calcPosX /= 4; }
//                int calcPosY = presenterPixmapSize.height();
//                if(calcPosY != 0) { calcPosY /= 4; }
//                targetRect = QRect(calcPosX, calcPosY, presenterPixmapSize.width(), presenterPixmapSize.height());
//            }
//        }

        // #2: calculate the position of the layer-image
        QPoint desiredLayerImagePos = QPoint(0, 0);
        if(currImageLayer.getAlignment() == ImageLayerAlignment::TopLeft) {
            // do nothing - default
        }
        else if(currImageLayer.getAlignment() == ImageLayerAlignment::TopRight) {
            desiredLayerImagePos = QPoint(presenterPixmapSize.width() - desiredLayerImageSize.width(), 0);
        }
        else if(currImageLayer.getAlignment() == ImageLayerAlignment::Center) {
            desiredLayerImagePos = QPoint( (presenterPixmapSize.width() / 2) - (currImageLayerImageSize.width() / 2),
                                           (presenterPixmapSize.height() / 2) - (currImageLayerImageSize.height() / 2) );
        }

        QRect targetRect = QRect(desiredLayerImagePos, desiredLayerImageSize);

        DLog("Target rect: ") << targetRect;
        picturePainter.drawImage(targetRect, currImageLayer.getImage());
    }

    picturePainter.end();
}
