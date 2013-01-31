#ifndef GRABSYNCAFTERLOGINACTION_H
#define GRABSYNCAFTERLOGINACTION_H

#include "afterloginactioninterface.h"

class GrabSyncAfterLoginAction : public AfterLoginActionInterface
{
public:
    explicit GrabSyncAfterLoginAction() {}
    QString getTypeId() const { return "GrabSyncAfterLoginAction"; }
};

#endif // GRABSYNCAFTERLOGINACTION_H
