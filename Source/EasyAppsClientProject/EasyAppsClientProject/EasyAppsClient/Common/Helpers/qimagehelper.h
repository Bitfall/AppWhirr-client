#ifndef QIMAGEHELPER_H
#define QIMAGEHELPER_H

#include <QImage>
#include <QPixmap>

#include "Common/imagetypesenum.h"

class QImageHelper
{
private:
    QImageHelper();

public:
    static QImage createImageByPath(QString imagePath, ImageTypes::ImageTypesEnum imageType);
    static QImage createImageByByteArray(const QByteArray &bytes, ImageTypes::ImageTypesEnum imageType);
    static QImage createImageByPixmap(QPixmap pixmap);
};

#endif // QIMAGEHELPER_H
