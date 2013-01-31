#ifndef IMAGEWITHTYPEINFO_H
#define IMAGEWITHTYPEINFO_H

#include <QString>
#include "imagetypesenum.h"

class ImageWithTypeInfo
{
public:
    ImageWithTypeInfo(QString imagePath, ImageTypes::ImageTypesEnum imageType);

    QString getImagePath() const;
    ImageTypes::ImageTypesEnum getImageType() const;

private:
    QString _imagePath;
    ImageTypes::ImageTypesEnum _imageType;
};

#endif // IMAGEWITHTYPEINFO_H
