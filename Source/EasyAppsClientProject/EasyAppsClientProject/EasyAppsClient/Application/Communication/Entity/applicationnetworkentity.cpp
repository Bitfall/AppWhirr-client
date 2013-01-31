#include "applicationnetworkentity.h"
#include "Application/Communication/Entity/applicationreviewentity.h"
#include "Application/Communication/Entity/applicationscreenshotentity.h"

ApplicationDetailInfosNetworkEntity::ApplicationDetailInfosNetworkEntity(QObject *parent) :
    ApplicationBaseInfosNetworkEntity(parent),
    applicationDescription("")
{
}

// ---------------
// --- setters ---

void ApplicationDetailInfosNetworkEntity::setApplicationDescription(QString applicationDescription) {
    this->applicationDescription = applicationDescription;
}

void ApplicationDetailInfosNetworkEntity::addApplicationImage(ApplicationScreenshotEntity* image) {
    this->applicationImages.append(image);
}

void ApplicationDetailInfosNetworkEntity::addApplicationVideo(QString video) {
    this->applicationVideos.append(video);
}

void ApplicationDetailInfosNetworkEntity::addApplicationReview(ApplicationReviewEntity* applicationReview) {
    this->applicationReviews.append(applicationReview);
}

void ApplicationDetailInfosNetworkEntity::setTwitterQuery(QString twitterQuery) {
    _twitterQuery = twitterQuery;
}

void ApplicationDetailInfosNetworkEntity::setDeveloperWebpageUrl(QString developerWebpageUrl) {
    _developerWebpageUrl = developerWebpageUrl;
}

void ApplicationDetailInfosNetworkEntity::setSupportUrl(QString supportUrl) {
    _supportUrl = supportUrl;
}

void ApplicationDetailInfosNetworkEntity::setPrice(QString price) {
    _price = price;
}

void ApplicationDetailInfosNetworkEntity::setRequiredHdd(QString requiredHdd) {
    _requiredHdd = requiredHdd;
}

// ---------------
// --- getters ---

QString ApplicationDetailInfosNetworkEntity::getApplicationDescription() {
    return applicationDescription;
}

QList<ApplicationScreenshotEntity*> ApplicationDetailInfosNetworkEntity::getApplicationImages() {
    return applicationImages;
}

QList<QString> ApplicationDetailInfosNetworkEntity::getApplicationVideos() {
    return applicationVideos;
}

QList<ApplicationReviewEntity*> ApplicationDetailInfosNetworkEntity::getApplicationReviews() {
    return applicationReviews;
}


QString ApplicationDetailInfosNetworkEntity::getTwitterQuery() const {
    return _twitterQuery;
}

QString ApplicationDetailInfosNetworkEntity::getDeveloperWebpageUrl() const {
    return _developerWebpageUrl;
}

QString ApplicationDetailInfosNetworkEntity::getSupportUrl() const {
    return _supportUrl;
}

QString ApplicationDetailInfosNetworkEntity::getPrice() const {
    return _price;
}

QString ApplicationDetailInfosNetworkEntity::getRequiredHdd() const {
    return _requiredHdd;
}
