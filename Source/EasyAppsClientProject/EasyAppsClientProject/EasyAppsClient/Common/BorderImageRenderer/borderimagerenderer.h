#ifndef BORDERIMAGERENDERER_H
#define BORDERIMAGERENDERER_H

#include <QPixmap>
#include <QPainter>
#include <QRect>

class BorderImageRenderer
{
private:
    BorderImageRenderer();
public:
    static void renderPixmapAsBorderImageToPainter(QPixmap sourcePixmap, QPainter &thePainter, QRect targetRect, int borderTop, int borderLeft, int borderBottom, int borderRigth);
};

#endif // BORDERIMAGERENDERER_H
