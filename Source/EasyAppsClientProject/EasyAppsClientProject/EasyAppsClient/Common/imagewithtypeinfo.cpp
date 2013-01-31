#include "imagewithtypeinfo.h"

ImageWithTypeInfo::ImageWithTypeInfo(QString imagePath, ImageTypes::ImageTypesEnum imageType):
    _imagePath(imagePath),
    _imageType(imageType)
{
}

QString ImageWithTypeInfo::getImagePath() const
{
    return this->_imagePath;
}

ImageTypes::ImageTypesEnum ImageWithTypeInfo::getImageType() const
{
    return this->_imageType;
}
