#include "applicationnetworkentity.h"
#include "Application/Communication/Entity/applicationreviewentity.h"
#include "Application/Communication/Entity/applicationscreenshotentity.h"

ApplicationNetworkEntity::ApplicationNetworkEntity(QObject *parent) :
    ApplicationBaseInfosNetworkEntity(parent),
    applicationDescription("")
{
}

void ApplicationNetworkEntity::setApplicationDescription(QString applicationDescription) {
    this->applicationDescription = applicationDescription;
}

void ApplicationNetworkEntity::addApplicationImage(ApplicationScreenshotEntity* image) {
    this->applicationImages.append(image);
}

void ApplicationNetworkEntity::addApplicationVideo(QString video) {
    this->applicationVideos.append(video);
}

void ApplicationNetworkEntity::addApplicationReview(ApplicationReviewEntity* applicationReview) {
    this->applicationReviews.append(applicationReview);
}

QString ApplicationNetworkEntity::getApplicationDescription() {
    return applicationDescription;
}

QList<ApplicationScreenshotEntity*> ApplicationNetworkEntity::getApplicationImages() {
    return applicationImages;
}

QList<QString> ApplicationNetworkEntity::getApplicationVideos() {
    return applicationVideos;
}

QList<ApplicationReviewEntity*> ApplicationNetworkEntity::getApplicationReviews() {
    return applicationReviews;
}
