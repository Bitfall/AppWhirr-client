#ifndef SHOWAPPDETAILPAGEFORAPPIDAFTERLOGINACTION_H
#define SHOWAPPDETAILPAGEFORAPPIDAFTERLOGINACTION_H

#include "afterloginactioninterface.h"

class ShowAppDetailPageForAppIdAfterLoginAction : public AfterLoginActionInterface
{
public:
    explicit ShowAppDetailPageForAppIdAfterLoginAction(QString appId) : _appId(appId) {}

    QString getTypeId() const { return "ShowAppDetailPageForAppIdAfterLoginAction"; }

    QString getAppId() const { return _appId; }

private:
    QString _appId;
};

#endif // SHOWAPPDETAILPAGEFORAPPIDAFTERLOGINACTION_H
