#ifndef BESTAPPLICATIONICONIMAGESELECTIONHELPER_H
#define BESTAPPLICATIONICONIMAGESELECTIONHELPER_H

#include "Common/imagewithtypeinfo.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class BestApplicationIconImageSelectionHelper
{
public:
    enum ImageSizeTypesEnum {
        Biggest,
        Smallest
    };

private:
    BestApplicationIconImageSelectionHelper();

public:
    static ImageWithTypeInfo getBestIconImageFromAppDescription(ApplicationDescriptionModel appDescriptionModel, ImageTypes::ImageTypesEnum preferredImageType, ImageSizeTypesEnum preferredImageSizeType);
};

#endif // BESTAPPLICATIONICONIMAGESELECTIONHELPER_H
