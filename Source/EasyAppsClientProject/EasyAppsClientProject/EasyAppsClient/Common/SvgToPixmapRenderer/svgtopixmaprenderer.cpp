#include "svgtopixmaprenderer.h"

#include <QtSvg/QSvgRenderer>
#include <QPainter>

SvgToPixmapRenderer::SvgToPixmapRenderer()
{
}

QPixmap SvgToPixmapRenderer::renderSvgFromFileOrResourceToPixmap(QString fileNameOrResourceAlias, QSize pixmapTargetSize)
{
    QPixmap _retPixmap;

    QSvgRenderer svgRenderer;
    svgRenderer.load(fileNameOrResourceAlias);

    if(svgRenderer.isValid())
    {
        // define a painter to render to QPicture
        //QPainter picturePainter(thePicture);
        _retPixmap = QPixmap(pixmapTargetSize);
        _retPixmap.fill(QColor(0, 0, 0, 0));
        //QImage tmpImg = QImage(150, 150, QImage::Format_ARGB32);
        QPainter picturePainter(&_retPixmap);
        //picturePainter.setBackground(QBrush(QColor(0, 0, 0, 0)));
        //picturePainter.setRenderHint(QPainter::SmoothPixmapTransform);
        picturePainter.fillRect(QRect(QPoint(0, 0), pixmapTargetSize), QColor(0, 0, 0, 0));
        svgRenderer.render(&picturePainter);
        picturePainter.end();
    }

    return _retPixmap;
}
