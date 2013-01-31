#ifndef RUNAPPAFTERLOGINACTION_H
#define RUNAPPAFTERLOGINACTION_H

#include "afterloginactioninterface.h"

class RunAppAfterLoginAction : public AfterLoginActionInterface {
public:
    explicit RunAppAfterLoginAction(QString appId) : _appId(appId) {}

    QString getTypeId() const { return "RunAppAfterLoginAction"; }

    QString getAppId() const { return _appId; }

private:
    QString _appId;
};

#endif // RUNAPPAFTERLOGINACTION_H
