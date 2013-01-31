#include "applicationbaseinfosnetworkentitymodel.h"

ApplicationBaseInfosNetworkEntityModel::ApplicationBaseInfosNetworkEntityModel() :
    applicationID(""),
    applicationName(""),
    applicationIconURL(""),
    applicationVersion(""),
    applicationCategory(""),
    applicationVersionSummedFlags(0),
    applicationRatingCount(0),
    applicationRating(0.0),
    applicationReviewCount(0),
    _appStatusInStore(AppWhirr::ApplicationStatusInStore::NoAppStatusInStore)
{
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationID(QString applicationID) {
    this->applicationID = applicationID;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationName(QString applicationName) {
    this->applicationName = applicationName;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationIconURL(QString applicationIconURL) {
    this->applicationIconURL = applicationIconURL;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationVersion(QString applicationVersion) {
    this->applicationVersion = applicationVersion;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationCategory(QString applicationCategory) {
    this->applicationCategory = applicationCategory;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationVersionSummedFlags(const int& applicationSummedFlags) {
    this->applicationVersionSummedFlags = applicationSummedFlags;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationRatingCount(const int& applicationRatingCount) {
    this->applicationRatingCount = applicationRatingCount;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationRating(const float& applicationRating) {
    this->applicationRating = applicationRating;
}

void ApplicationBaseInfosNetworkEntityModel::setApplicationReviewCount(const int& applicationReviewCount) {
    this->applicationReviewCount = applicationReviewCount;
}

void ApplicationBaseInfosNetworkEntityModel::setAppStatusInStore(AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore) {
    this->_appStatusInStore = appStatusInStore;
}


//
// getters


QString ApplicationBaseInfosNetworkEntityModel::getApplicationID() {
    return applicationID;
}

QString ApplicationBaseInfosNetworkEntityModel::getApplicationName() {
    return applicationName;
}

QString ApplicationBaseInfosNetworkEntityModel::getApplicationVersion() {
    return applicationVersion;
}

QString ApplicationBaseInfosNetworkEntityModel::getApplicationIconURL() {
    return applicationIconURL;
}

QString ApplicationBaseInfosNetworkEntityModel::getApplicationCategory() {
    return applicationCategory;
}

int ApplicationBaseInfosNetworkEntityModel::getApplicationVersionSummedFlags() {
    return applicationVersionSummedFlags;
}

int ApplicationBaseInfosNetworkEntityModel::getApplicationRatingCount() {
    return applicationRatingCount;
}

float ApplicationBaseInfosNetworkEntityModel::getApplicationRating() {
    return applicationRating;
}

int ApplicationBaseInfosNetworkEntityModel::getApplicationReviewCount() {
    return applicationReviewCount;
}

AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum ApplicationBaseInfosNetworkEntityModel::getAppStatusInStore() {
    return _appStatusInStore;
}
