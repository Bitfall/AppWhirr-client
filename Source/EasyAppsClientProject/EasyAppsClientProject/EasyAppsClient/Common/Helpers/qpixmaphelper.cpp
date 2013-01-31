#include "qpixmaphelper.h"

QPixmapHelper::QPixmapHelper()
{
}

QPixmap QPixmapHelper::createPixmapFromByteArray(QByteArray pixmapByteArray) {
    return QPixmap::fromImage(QImage::fromData(pixmapByteArray));
}
