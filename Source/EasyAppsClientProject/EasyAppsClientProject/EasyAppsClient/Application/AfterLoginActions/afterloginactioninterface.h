#ifndef AFTERLOGINACTIONINTERFACE_H
#define AFTERLOGINACTIONINTERFACE_H

#include <QString>

class AfterLoginActionInterface {
public:
    virtual ~AfterLoginActionInterface() {}

    //
    // type-id is a unique after-login-action CLASS(!) type id - will be used to identify an action type and cast it to the appropriate type
    virtual QString getTypeId() const = 0; // example: { return "AfterLoginActionInterface"; }
};

#endif // AFTERLOGINACTIONINTERFACE_H
