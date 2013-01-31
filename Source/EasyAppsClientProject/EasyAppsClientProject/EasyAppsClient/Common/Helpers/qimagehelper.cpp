#include "qimagehelper.h"

QImageHelper::QImageHelper()
{
}

QImage QImageHelper::createImageByByteArray(const QByteArray &bytes, ImageTypes::ImageTypesEnum imageType)
{
    QImage img;
    img.loadFromData(bytes);
    return img;
}

QImage QImageHelper::createImageByPath(QString imagePath, ImageTypes::ImageTypesEnum imageType)
{
    QImage img;
    img.load(imagePath);
    return img;
}

QImage QImageHelper::createImageByPixmap(QPixmap pixmap)
{
    return pixmap.toImage();
}
