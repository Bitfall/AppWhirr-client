#include "applicationscreenshotentity.h"

ApplicationScreenshotEntity::ApplicationScreenshotEntity(QObject* parent)
    : BaseNetworkEntity(parent),
      screenshotUrl(""),
      thumbnailUrl("")
{
}

void ApplicationScreenshotEntity::setScreenshotUrl(QString screenshotUrl) {
    this->screenshotUrl = screenshotUrl;
}

void ApplicationScreenshotEntity::setThumbnailUrl(QString thumbnailUrl) {
    this->thumbnailUrl = thumbnailUrl;
}

QString ApplicationScreenshotEntity::getScreenshotUrl() {
    return this->screenshotUrl;
}

QString ApplicationScreenshotEntity::getThumbnailUrl() {
    return this->thumbnailUrl;
}
