#ifndef LAYEREDIMAGEPRESENTERWIDGET_H
#define LAYEREDIMAGEPRESENTERWIDGET_H

#include <QWidget>
#include <QImage>
#include <QSvgRenderer>
#include <QPixmap>

#include "Common/imagetypesenum.h"

/*! Layered image-presenter widget

    @note Currently only PixelBased images supported (no SVG support)

    @note Base-image-layer: the first / bottom-most image-layer -> which will be rendered first
*/
class LayeredImagePresenterWidget : public QWidget
{
    Q_OBJECT

public:
    class ImageLayerAlignment
    {
    public:
        enum ImageLayerAlignmentEnum {
            Center,
            TopLeft,
            TopRight
        };
    };

    class ImageLayerResizeMode {
    public:
        enum ImageLayerResizeModeEnum {
            NoResize,
            FitToSpace, // the layer's proportion will be the same - proportional resize
            FillTheSpace
        };
    };

    class ImageLayerModel
    {
    public:
        explicit ImageLayerModel() : _alignment(ImageLayerAlignment::TopLeft), _resizeMode(ImageLayerResizeMode::NoResize) {} // empty / default constructor for Qt meta object system
        explicit ImageLayerModel(QImage image, ImageLayerAlignment::ImageLayerAlignmentEnum alignment, ImageLayerResizeMode::ImageLayerResizeModeEnum resizeMode) :
            _image(image), _alignment(alignment), _resizeMode(resizeMode) {}

        QImage getImage() const { return _image; }
        ImageLayerAlignment::ImageLayerAlignmentEnum getAlignment() const { return _alignment; }
        ImageLayerResizeMode::ImageLayerResizeModeEnum getResizeMode() const { return _resizeMode; }

    private:
        QImage _image;
        ImageLayerAlignment::ImageLayerAlignmentEnum _alignment;
        ImageLayerResizeMode::ImageLayerResizeModeEnum _resizeMode;
    };

public:
    explicit LayeredImagePresenterWidget(QWidget *parent = 0);
    
    void setIsAutoResizeToBaseImageLayerSize(bool isAutoResize);
    void addImageLayer(ImageLayerModel imageLayer, bool isForceUpdateAfterAdd = true);
    void setImageLayer(int layerIndex, ImageLayerModel imageLayer);

    QPixmap getPresentedImageAsPixmap() const;

Q_SIGNALS:
    void clicked();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual bool event(QEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    void _refreshRenderPixmap();

    void forceFullRepaint();
    void flagFullRepaintNeeded();
    bool getIsFullRepaintNeeded() const;

private:
    void resizeToBaseImageLayerSize();

private:
    QPixmap _pixmapToPresent;
    QList<ImageLayerModel> _imageLayers;

    bool _isAutoResizeToImageSize;
    bool _isFullRepaintNeeded;
};

#endif // LAYEREDIMAGEPRESENTERWIDGET_H
