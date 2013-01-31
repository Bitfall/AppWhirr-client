#ifndef SIMPLEIMAGEPRESENTERWIDGET_H
#define SIMPLEIMAGEPRESENTERWIDGET_H

#include <QWidget>
#include <QImage>
#include <QSvgRenderer>
#include <QPixmap>

#include "Common/imagetypesenum.h"

class SimpleImagePresenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleImagePresenterWidget(QWidget *parent = 0);

    void setIsAutoResizeToImageSize(bool isAutoResize);

    void setImageByPath(QString imagePath, ImageTypes::ImageTypesEnum imageType);
    void setImageByByteArray(const QByteArray &bytes, ImageTypes::ImageTypesEnum imageType);
    void setImageByPixmap(QPixmap pixmap);
//    void setImageByImage(QImage image, ImageTypes::ImageTypesEnum imageType);

    void addImageLayerByPath(QString imagePath, ImageTypes::ImageTypesEnum imageType);

    QPixmap getPresentedImageAsPixmap();
    bool getIsImageDefined() const;
//    QByteArray getImageBytes();

Q_SIGNALS:
    void clicked();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual bool event(QEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    void _refreshPresentablePixmapByRenderImage();

    void forceFullRepaint();
    void flagFullRepaintNeeded();
    bool getIsFullRepaintNeeded() const;

private:
    void resizeToImageSize();

//    void loadPixelBasedImage(const QString &fileNameOrAlias);
//    void loadSvgImage(const QString &fileNameOrAlias);

protected:
    QPixmap _pixmapToPresent;
    QImage _renderImage;
    QSvgRenderer _svgRenderer;

    ImageTypes::ImageTypesEnum _currImageType;

private:
//    QByteArray bytes;
    bool _isAutoResizeToImageSize;
    bool _isFullRepaintNeeded;
};

#endif // SIMPLEIMAGEPRESENTERWIDGET_H
