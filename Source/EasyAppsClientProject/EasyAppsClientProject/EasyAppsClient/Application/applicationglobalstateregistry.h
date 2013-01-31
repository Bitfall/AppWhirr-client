#ifndef APPLICATIONGLOBALSTATEREGISTRY_H
#define APPLICATIONGLOBALSTATEREGISTRY_H

#include <QList>

class AfterLoginActionInterface;

class ApplicationGlobalStateRegistry
{
private:
    explicit ApplicationGlobalStateRegistry();
    ~ApplicationGlobalStateRegistry();

public:
    static ApplicationGlobalStateRegistry* sharedApplicationGlobalStateRegistry();

    bool getIsUpdateChekingAndInitialLoginFinished() const;
    void setIsUpdateChekingAndInitialLoginFinished(bool value);

//    bool getIsGrabListSyncRequired() const;
//    void setIsGrabListSyncRequired(bool value);

    bool getIsInInitialUserAppLoading() const;
    void setIsInInitialUserAppLoading(bool value);

    //
    // after login actions
    //  !!! Use them with caution
    //      * not thread safe
    void addAfterLoginAction(AfterLoginActionInterface *action);
    QList<AfterLoginActionInterface *> getAfterLoginActions() const;
    void deleteAndClearAfterLoginActions();

private:
    bool _isUpdateChekingAndInitialLoginFinished;
//    bool _isGrabListSyncRequired;
    bool _isInInitialUserAppLoading;

    QList<AfterLoginActionInterface *> _afterLoginActions;
};

#endif // APPLICATIONGLOBALSTATEREGISTRY_H
