#ifndef OFFLINEUSERAPPLISTUPDATECHECKAFTERLOGINACTION_H
#define OFFLINEUSERAPPLISTUPDATECHECKAFTERLOGINACTION_H

#include "afterloginactioninterface.h"

class OfflineUserAppListUpdateCheckAfterLoginAction : public AfterLoginActionInterface
{
public:
    explicit OfflineUserAppListUpdateCheckAfterLoginAction() {}
    QString getTypeId() const { return "OfflineUserAppListUpdateCheckAfterLoginAction"; }
};

#endif // OFFLINEUSERAPPLISTUPDATECHECKAFTERLOGINACTION_H
