#include "borderimagerenderer.h"

BorderImageRenderer::BorderImageRenderer()
{
}

void BorderImageRenderer::renderPixmapAsBorderImageToPainter(QPixmap sourcePixmap, QPainter &thePainter, QRect targetRect, int borderTop, int borderLeft, int borderBottom, int borderRigth)
{
    // TODO: some error checking, for the sizes


    // --- corners ---

    // top-left corner
    thePainter.drawPixmap(targetRect.x(), targetRect.y(), sourcePixmap, 0, 0, borderLeft, borderTop);

    // top-right corner
    thePainter.drawPixmap(targetRect.x() + targetRect.width() - borderRigth, // x
                       targetRect.y(), // y
                       sourcePixmap, // pixmap
                       sourcePixmap.width() - borderRigth, // pixmap x
                       0, // pixmap y
                       borderRigth, // pixmap width
                       borderTop); // pixmap height

    // bottom-left corner
    thePainter.drawPixmap(targetRect.x(), // x
                       targetRect.y() + targetRect.height() - borderBottom, // y
                       sourcePixmap, // pixmap
                       0, // pixmap x
                       sourcePixmap.height() - borderBottom, // pixmap y
                       borderLeft, // pixmap width
                       borderBottom); // pixmap height

    // bottom-right corner
    thePainter.drawPixmap(targetRect.x() + targetRect.width() - borderRigth, // x
                       targetRect.y() + targetRect.height() - borderBottom, // y
                       sourcePixmap, // pixmap
                       sourcePixmap.width() - borderRigth, // pixmap x
                       sourcePixmap.height() - borderBottom, // pixmap y
                       borderRigth, // pixmap width
                       borderBottom); // pixmap height

    // --- borders ---

    // left
    thePainter.drawPixmap(targetRect.x(), // x
                       targetRect.y() + borderTop, // y
                       borderLeft, // width
                       targetRect.height() - borderTop - borderBottom, // height
                       sourcePixmap, // pixmap
                       0, // pixmap x
                       borderTop, // pixmap y
                       borderLeft, // pixmap width
                       sourcePixmap.height() - borderTop - borderBottom); // pixmap height

    // right
    thePainter.drawPixmap(targetRect.x() + targetRect.width() - borderRigth, // x
                       targetRect.y() + borderTop, // y
                       borderRigth, // width
                       targetRect.height() - borderTop - borderBottom, // height
                       sourcePixmap, // pixmap
                       sourcePixmap.width() - borderRigth, // pixmap x
                       borderTop, // pixmap y
                       borderRigth, // pixmap width
                       sourcePixmap.height() - borderTop - borderBottom); // pixmap height

    // top
    thePainter.drawPixmap(targetRect.x() + borderLeft, // x
                       targetRect.y(), // y
                       targetRect.width() - borderLeft - borderRigth, // width
                       borderTop, // height
                       sourcePixmap, // pixmap
                       borderLeft, // pixmap x
                       0, // pixmap y
                       sourcePixmap.width() - borderLeft - borderRigth, // pixmap width
                       borderTop); // pixmap height

    // bottom
    thePainter.drawPixmap(targetRect.x() + borderLeft, // x
                       targetRect.y() + targetRect.height() - borderBottom, // y
                       targetRect.width() - borderLeft - borderRigth, // width
                       borderBottom, // height
                       sourcePixmap, // pixmap
                       borderLeft, // pixmap x
                       sourcePixmap.height() - borderBottom, // pixmap y
                       sourcePixmap.width() - borderLeft - borderRigth, // pixmap width
                       borderBottom); // pixmap height

    // --- center ---

    // center
    thePainter.drawPixmap(targetRect.x() + borderLeft, // x
                       targetRect.y() + borderTop, // y
                       targetRect.width() - borderLeft - borderRigth, // width
                       targetRect.height() - borderTop - borderBottom, // height
                       sourcePixmap, // pixmap
                       borderLeft, // pixmap x
                       borderTop, // pixmap y
                       sourcePixmap.width() - borderLeft - borderRigth, // pixmap width
                       sourcePixmap.height() - borderTop - borderBottom); // pixmap height
}
