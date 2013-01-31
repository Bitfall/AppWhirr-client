#include "simpleimagepresenterwidget.h"

#include <QPainter>
#include <QMouseEvent>

SimpleImagePresenterWidget::SimpleImagePresenterWidget(QWidget *parent) :
    QWidget(parent),
    _isAutoResizeToImageSize(false),
    _isFullRepaintNeeded(false)
{
    this->setAttribute(Qt::WA_NoSystemBackground, true);
}

void SimpleImagePresenterWidget::resizeEvent(QResizeEvent *e) {
    this->_isFullRepaintNeeded = true;

    QWidget::resizeEvent(e);
}

void SimpleImagePresenterWidget::paintEvent(QPaintEvent *e)
{
    if(_isFullRepaintNeeded) {
        this->_refreshPresentablePixmapByRenderImage();
        this->_isFullRepaintNeeded = false;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(this->rect(), _pixmapToPresent);
}

bool SimpleImagePresenterWidget::event(QEvent *e)
{
    if(e->type() == QEvent::MouseButtonRelease)
    {
        Q_EMIT clicked();
        e->accept();
        return true;
    }

    return QWidget::event(e);
}

void SimpleImagePresenterWidget::setImageByPath(QString imagePath, ImageTypes::ImageTypesEnum imageType) {
    this->_currImageType = imageType;

    switch(imageType)
    {
    case ImageTypes::PixelBased:
        _renderImage.load(imagePath);
        break;
    case ImageTypes::SvgBased:
        _svgRenderer.load(imagePath);
        break;
    default:
        WLog("This image-type is not allowed");
        break;
    }

    this->_refreshPresentablePixmapByRenderImage();

    if(this->_isAutoResizeToImageSize) {
        this->resizeToImageSize();
    }

//    this->update();
    this->forceFullRepaint();
}

void SimpleImagePresenterWidget::setImageByByteArray(const QByteArray &bytes, ImageTypes::ImageTypesEnum imageType)
{
    this->_currImageType = imageType;
//    this->bytes = bytes;

    switch(imageType)
    {
    case ImageTypes::PixelBased:
        _renderImage.loadFromData(bytes);
        break;
    case ImageTypes::SvgBased:
        _svgRenderer.load(bytes);
        break;
    default:
        qWarning("This image-type is not allowed");
        break;
    }

    this->_refreshPresentablePixmapByRenderImage();

    if(this->_isAutoResizeToImageSize) {
        this->resizeToImageSize();
    }

//    this->update();
    this->forceFullRepaint();
}

void SimpleImagePresenterWidget::setImageByPixmap(QPixmap pixmap)
{
    this->_currImageType = ImageTypes::PixelBased;

    this->_pixmapToPresent = pixmap;
    this->_renderImage = QImage(); // set it to empty, which will return isNull() == true and won't be rendered to the pixmap - this way the pixmap will be used directly

    if(this->_isAutoResizeToImageSize) {
        this->resizeToImageSize();
    }

//    this->update();
    this->forceFullRepaint();
}

void SimpleImagePresenterWidget::setIsAutoResizeToImageSize(bool isAutoResize)
{
    _isAutoResizeToImageSize = isAutoResize;
    if(_isAutoResizeToImageSize) {
        this->resizeToImageSize();
    }

    this->update();
}

void SimpleImagePresenterWidget::resizeToImageSize() {
//    DLog(" -- Resize to image size");
    if(!_pixmapToPresent.isNull()) {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->setFixedSize(_pixmapToPresent.size());
        this->updateGeometry();
    }
}

QPixmap SimpleImagePresenterWidget::getPresentedImageAsPixmap()
{
    return this->_pixmapToPresent;
}

bool SimpleImagePresenterWidget::getIsImageDefined() const
{
    return !(this->_pixmapToPresent.isNull());
}

void SimpleImagePresenterWidget::forceFullRepaint() {
    this->flagFullRepaintNeeded();
    this->repaint();
}

void SimpleImagePresenterWidget::flagFullRepaintNeeded() {
    this->_isFullRepaintNeeded = true;
}

bool SimpleImagePresenterWidget::getIsFullRepaintNeeded() const {
    return this->_isFullRepaintNeeded;
}

void SimpleImagePresenterWidget::_refreshPresentablePixmapByRenderImage()
{
    /*QRectF testRect(30, 30, this->width() - 1 - 30, this->height() - 1 - 30);
    QRect testImageRect(30, 30, this->width() - 1 - 30 - 30, this->height() - 1 - 30 - 30);*/
    //QRect targetRect(0, 0, GuiWidgetSizeRegistry::iconImageWidgetGrownSize());


    //QTransform transf;
    //painter.setTransform(transf.rotate(45, Qt::XAxis));
    //painter.setTransform(transf.rotate(45, Qt::YAxis));
    //painter.setTransform(transf.rotate(30, Qt::ZAxis));
    //painter.setRenderHint(QPainter::SmoothPixmapTransform);
    //painter.setRenderHint(QPainter::Antialiasing);


    /*
    if(thePicture != NULL)
    {
        thePicture->setBoundingRect(targetRect);
    }
    */


#if 0
    if(!finalPixmap.isNull())
    {
        finalPixmap->scaled(targetRect.size());
    }
#endif



    // SVG
#if 1
    if(_currImageType == ImageTypes::SvgBased && _svgRenderer.isValid())
    {
        // define a painter to render to QPicture
        //QPainter picturePainter(thePicture);
        this->_pixmapToPresent = QPixmap(this->size());
        this->_pixmapToPresent.fill(QColor(0, 0, 0, 0));
        //QImage tmpImg = QImage(150, 150, QImage::Format_ARGB32);
        QPainter picturePainter(&(this->_pixmapToPresent));
        //picturePainter.setBackground(QBrush(QColor(0, 0, 0, 0)));
        //picturePainter.setRenderHint(QPainter::SmoothPixmapTransform);
        picturePainter.fillRect(QRect(QPoint(0, 0), this->size()), QColor(0, 0, 0, 0));
        _svgRenderer.render(&picturePainter);
        picturePainter.end();

        //finalPixmap =
    }
#endif

    // Pixel Based Image
    if(!_renderImage.isNull() && _currImageType == ImageTypes::PixelBased)
    {
#if 0
        finalPixmap = QPixmap(150, 150);
        //QPainter picturePainter(thePicture);
        QPainter picturePainter(&finalPixmap);
        picturePainter.fillRect(QRect(0, 0, 150, 150), QColor(0, 0, 0, 0));
        //picturePainter.setBackground(QBrush(QColor(0, 0, 0, 0)));
        //picturePainter.fillRect(QRect(0, 0, 150, 150), QColor(0, 0, 0, 0));
        picturePainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        picturePainter.setBackgroundMode(Qt::TransparentMode);
        picturePainter.setRenderHint(QPainter::SmoothPixmapTransform);
        picturePainter.drawImage(QRect(0, 0, 150, 150), theImage, QRect(0, 0, 256, 256), Qt::DiffuseAlphaDither);
        picturePainter.end();
#endif
        this->_pixmapToPresent = QPixmap::fromImage(_renderImage);
        //finalPixmap.convertFromImage(theImage, Qt::NoOpaqueDetection);
    }

#if 0
    if(svgRenderer != NULL)
    {
        // define a painter to render to QPicture
        QPainter imagePainter(theImage);
        svgRenderer->render(&imagePainter);
        imagePainter.end();
    }
#endif
}

//QByteArray SimpleImagePresenterWidget::getImageBytes() {
//    return bytes;
//}
