#include "applicationbaseinfosnetworkentity.h"

ApplicationBaseInfosNetworkEntity::ApplicationBaseInfosNetworkEntity(QObject *parent) :
    BaseNetworkEntity(parent)
{
}

void ApplicationBaseInfosNetworkEntity::setApplicationID(QString applicationID) {
    _model.setApplicationID(applicationID);
}

void ApplicationBaseInfosNetworkEntity::setApplicationName(QString applicationName) {
    _model.setApplicationName(applicationName);
}

void ApplicationBaseInfosNetworkEntity::setApplicationIconURL(QString applicationIconURL) {
    _model.setApplicationIconURL(applicationIconURL);
}

void ApplicationBaseInfosNetworkEntity::setApplicationVersion(QString applicationVersion) {
    _model.setApplicationVersion(applicationVersion);
}

void ApplicationBaseInfosNetworkEntity::setApplicationCategory(QString applicationCategory) {
    _model.setApplicationCategory(applicationCategory);
}

void ApplicationBaseInfosNetworkEntity::setApplicationVersionSummedFlags(const int& applicationSummedFlags) {
    _model.setApplicationVersionSummedFlags(applicationSummedFlags);
}

void ApplicationBaseInfosNetworkEntity::setApplicationRatingCount(const int& applicationRatingCount) {
    _model.setApplicationRatingCount(applicationRatingCount);
}

void ApplicationBaseInfosNetworkEntity::setApplicationRating(const float& applicationRating) {
    _model.setApplicationRating(applicationRating);
}

void ApplicationBaseInfosNetworkEntity::setApplicationReviewCount(const int& applicationReviewCount) {
    _model.setApplicationReviewCount(applicationReviewCount);
}

void ApplicationBaseInfosNetworkEntity::setAppStatusInStore(AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore) {
    _model.setAppStatusInStore(appStatusInStore);
}

//
// getters

QString ApplicationBaseInfosNetworkEntity::getApplicationID() {
    return _model.getApplicationID();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationName() {
    return _model.getApplicationName();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationVersion() {
    return _model.getApplicationVersion();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationIconURL() {
    return _model.getApplicationIconURL();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationCategory() {
    return _model.getApplicationCategory();
}

int ApplicationBaseInfosNetworkEntity::getApplicationVersionSummedFlags() {
    return _model.getApplicationVersionSummedFlags();
}

int ApplicationBaseInfosNetworkEntity::getApplicationRatingCount() {
    return _model.getApplicationRatingCount();
}

float ApplicationBaseInfosNetworkEntity::getApplicationRating() {
    return _model.getApplicationRating();
}

int ApplicationBaseInfosNetworkEntity::getApplicationReviewCount() {
    return _model.getApplicationReviewCount();

}

AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum ApplicationBaseInfosNetworkEntity::getAppStatusInStore() {
    return _model.getAppStatusInStore();
}




ApplicationBaseInfosNetworkEntityModel ApplicationBaseInfosNetworkEntity::getModel() const {
    return _model;
}
