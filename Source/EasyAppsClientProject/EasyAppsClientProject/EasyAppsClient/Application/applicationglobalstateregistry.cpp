#include "applicationglobalstateregistry.h"


ApplicationGlobalStateRegistry* ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()
{
    static ApplicationGlobalStateRegistry* applicationGlobalStateRegistry = NULL;

    if (applicationGlobalStateRegistry == NULL) {
        applicationGlobalStateRegistry = new ApplicationGlobalStateRegistry();
    }
    return applicationGlobalStateRegistry;
}

ApplicationGlobalStateRegistry::ApplicationGlobalStateRegistry() :
    _isUpdateChekingAndInitialLoginFinished(false),
//    _isGrabListSyncRequired(false),
    _isInInitialUserAppLoading(false)
{}

bool ApplicationGlobalStateRegistry::getIsUpdateChekingAndInitialLoginFinished() const {
    return this->_isUpdateChekingAndInitialLoginFinished;
}

void ApplicationGlobalStateRegistry::setIsUpdateChekingAndInitialLoginFinished(bool value) {
    this->_isUpdateChekingAndInitialLoginFinished = value;
}

//bool ApplicationGlobalStateRegistry::getIsGrabListSyncRequired() const {
//    return this->_isGrabListSyncRequired;
//}

//void ApplicationGlobalStateRegistry::setIsGrabListSyncRequired(bool value) {
//    this->_isGrabListSyncRequired = value;
//}

bool ApplicationGlobalStateRegistry::getIsInInitialUserAppLoading() const {
    return this->_isInInitialUserAppLoading;
}

void ApplicationGlobalStateRegistry::setIsInInitialUserAppLoading(bool value) {
    this->_isInInitialUserAppLoading = value;
}


void ApplicationGlobalStateRegistry::addAfterLoginAction(AfterLoginActionInterface *action) {
    this->_afterLoginActions.append(action);
}

QList<AfterLoginActionInterface *> ApplicationGlobalStateRegistry::getAfterLoginActions() const {
    return this->_afterLoginActions;
}
void ApplicationGlobalStateRegistry::deleteAndClearAfterLoginActions() {
    for(int i = 0; i < this->_afterLoginActions.size(); i++) {
        delete this->_afterLoginActions[i];
    }
    this->_afterLoginActions.clear();
}
