#include "baseupdater.h"

BaseUpdater::BaseUpdater(QObject *parent) :
    QObject(parent)
{
    LOG_AS_NOT_IMPLEMENTED;
}

void BaseUpdater::checkForUpdates() {
    // Do checking
    LOG_AS_NOT_IMPLEMENTED;
}

void BaseUpdater::setURLForUpdateInformation(const QString& url) {
    LOG_AS_NOT_IMPLEMENTED;
}

QString BaseUpdater::urlForUpdateInformation() {
    LOG_AS_NOT_IMPLEMENTED;
    return QString();
}

void BaseUpdater::setAutomaticallyCheckForUpdates(bool checkAutomatically) {
    LOG_AS_NOT_IMPLEMENTED;
}

bool BaseUpdater::isAutomaticallyCheckingForUpdates() {
    LOG_AS_NOT_IMPLEMENTED;
    return false;
}

void BaseUpdater::setUpdateCheckInterval(int interval) {
    LOG_AS_NOT_IMPLEMENTED;
}

int BaseUpdater::updateCheckInterval() {
    LOG_AS_NOT_IMPLEMENTED;
    return 10000;
}
