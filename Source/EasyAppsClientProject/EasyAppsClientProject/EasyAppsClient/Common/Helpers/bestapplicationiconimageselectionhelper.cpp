#include "bestapplicationiconimageselectionhelper.h"

// utility
#include "Common/Helpers/pathhelper.h"

BestApplicationIconImageSelectionHelper::BestApplicationIconImageSelectionHelper()
{
}

ImageWithTypeInfo BestApplicationIconImageSelectionHelper::getBestIconImageFromAppDescription(ApplicationDescriptionModel appDescriptionModel, ImageTypes::ImageTypesEnum preferredImageType, ImageSizeTypesEnum preferredImageSizeType)
{
    ImageWithTypeInfo _ret(QString(), ImageTypes::PixelBased);

    bool isBestFound = false;
    bool isIconFound = false;

    if(!appDescriptionModel.getStaticDescription_ReadOnly()->getSvgIconPath().isEmpty())
    {
        QString path = PathHelper::combineAndCleanPathes(appDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescriptionModel.getStaticDescription_ReadOnly()->getSvgIconPath());
        _ret = ImageWithTypeInfo(path, ImageTypes::SvgBased);

        if(preferredImageType == ImageTypes::SvgBased) {
            isBestFound = true;
        }

        isIconFound = true;
    }

    if(!isBestFound && !appDescriptionModel.getStaticDescription_ReadOnly()->getLargeIconPath().isEmpty())
    {
        QString path = PathHelper::combineAndCleanPathes(appDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescriptionModel.getStaticDescription_ReadOnly()->getLargeIconPath());
        _ret = ImageWithTypeInfo(path, ImageTypes::PixelBased);

        if(preferredImageSizeType == Biggest) {
            isBestFound = true;
        }

        isIconFound = true;
    }

    if(!isBestFound && !appDescriptionModel.getStaticDescription_ReadOnly()->getMediumIconPath().isEmpty())
    {
        QString path = PathHelper::combineAndCleanPathes(appDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescriptionModel.getStaticDescription_ReadOnly()->getMediumIconPath());
        _ret = ImageWithTypeInfo(path, ImageTypes::PixelBased);

        isIconFound = true;
    }

    if(!isBestFound && !appDescriptionModel.getStaticDescription_ReadOnly()->getSmallIconPath().isEmpty())
    {
        QString path = PathHelper::combineAndCleanPathes(appDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescriptionModel.getStaticDescription_ReadOnly()->getSmallIconPath());
        _ret = ImageWithTypeInfo(path, ImageTypes::PixelBased);

        if(preferredImageSizeType == Smallest) {
            isBestFound = true;
        }

        isIconFound = true;
    }

    if(!isIconFound && !appDescriptionModel.getDynamicDescription_ReadOnly()->getGrabbedIconPath().isEmpty())
    {
        QString path = appDescriptionModel.getDynamicDescription_ReadOnly()->getGrabbedIconPath();
        _ret = ImageWithTypeInfo(path, ImageTypes::PixelBased);
    }

    return _ret;
}
