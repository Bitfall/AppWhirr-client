#include "applicationscreenshotentity.h"

ApplicationScreenshotEntity::ApplicationScreenshotEntity(QObject* parent)
    : AW::Common::Communication::BaseNetworkEntity(parent),
      screenshotUrl(""),
      thumbnailUrl("")
{
}

void ApplicationScreenshotEntity::setScreenshotUrl(QString screenshotUrl) {
    // Append https if necessary
    this->screenshotUrl = (screenshotUrl.startsWith("//")) ? QString("https:").append(screenshotUrl) : screenshotUrl;
}

void ApplicationScreenshotEntity::setThumbnailUrl(QString thumbnailUrl) {
    // Append https if necessary
    this->thumbnailUrl = (thumbnailUrl.startsWith("//")) ? QString("https:").append(thumbnailUrl) : thumbnailUrl;
}

QString ApplicationScreenshotEntity::getScreenshotUrl() {
    return this->screenshotUrl;
}

QString ApplicationScreenshotEntity::getThumbnailUrl() {
    return this->thumbnailUrl;
}
