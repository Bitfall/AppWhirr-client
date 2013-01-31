#include "applicationbaseinfosnetworkentity.h"

ApplicationBaseInfosNetworkEntity::ApplicationBaseInfosNetworkEntity(QObject *parent) :
    AW::Common::Communication::BaseNetworkEntity(parent)
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

void ApplicationBaseInfosNetworkEntity::setApplicationSupportURL(QString applicationSupportURL) {
    _model.setApplicationSupportURL(applicationSupportURL);
}

void ApplicationBaseInfosNetworkEntity::setApplicationDeveloperURL(QString applicationDeveloperURL) {
    _model.setApplicationDeveloperURL(applicationDeveloperURL);
}

void ApplicationBaseInfosNetworkEntity::setApplicationWindowsReadableFlags(QString applicationWindowsReadableFlags) {
    _model.setApplicationWindowsReadableFlags(applicationWindowsReadableFlags);
}

void ApplicationBaseInfosNetworkEntity::setApplicationMacReadableFlags(QString applicationMacReadableFlags) {
    _model.setApplicationMacReadableFlags(applicationMacReadableFlags);
}

void ApplicationBaseInfosNetworkEntity::setApplicationVersionSummedFlags(quint64 applicationSummedFlags) {
    _model.setApplicationVersionSummedFlags(applicationSummedFlags);
}

void ApplicationBaseInfosNetworkEntity::setApplicationSize(int applicationSize) {
    _model.setApplicationSize(applicationSize);
}

void ApplicationBaseInfosNetworkEntity::setApplicationRatingCount(quint32 applicationRatingCount) {
    _model.setApplicationRatingCount(applicationRatingCount);
}

void ApplicationBaseInfosNetworkEntity::setApplicationRating(float applicationRating) {
    _model.setApplicationRating(applicationRating);
}

void ApplicationBaseInfosNetworkEntity::setApplicationReviewCount(quint32 applicationReviewCount) {
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

QString ApplicationBaseInfosNetworkEntity::getApplicationSupportURL() {
    return _model.getApplicationSupportURL();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationDeveloperURL() {
    return _model.getApplicationDeveloperURL();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationWindowsReadableFlags() {
    return _model.getApplicationWindowsReadableFlags();
}

QString ApplicationBaseInfosNetworkEntity::getApplicationMacReadableFlags() {
    return _model.getApplicationMacReadableFlags();
}

quint64 ApplicationBaseInfosNetworkEntity::getApplicationVersionSummedFlags() {
    return _model.getApplicationVersionSummedFlags();
}

int ApplicationBaseInfosNetworkEntity::getApplicationSize() {
    return _model.getApplicationSize();
}

quint32 ApplicationBaseInfosNetworkEntity::getApplicationRatingCount() {
    return _model.getApplicationRatingCount();
}

float ApplicationBaseInfosNetworkEntity::getApplicationRating() {
    return _model.getApplicationRating();
}

quint32 ApplicationBaseInfosNetworkEntity::getApplicationReviewCount() {
    return _model.getApplicationReviewCount();

}

AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum ApplicationBaseInfosNetworkEntity::getAppStatusInStore() {
    return _model.getAppStatusInStore();
}




ApplicationBaseInfosNetworkEntityModel ApplicationBaseInfosNetworkEntity::getModel() const {
    return _model;
}
