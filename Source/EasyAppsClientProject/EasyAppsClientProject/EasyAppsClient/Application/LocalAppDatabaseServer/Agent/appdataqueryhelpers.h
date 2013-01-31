#ifndef APPDATAQUERYHELPERS_H
#define APPDATAQUERYHELPERS_H

#include <QSqlDatabase>
#include <QString>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class AppDataQueryHelpers
{
private:
    AppDataQueryHelpers() {}

public:    
    static bool getStaticAppDataByAppId(QSqlDatabase database, QString appId, ApplicationDescriptionModel &outResult);
    static bool getDynamicAppDataByAppId(QSqlDatabase database, QString appId, ApplicationDescriptionModel &outResult);
    static bool getAppStateByAppId(QSqlDatabase database, QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum &outResult);
};

#endif // APPDATAQUERYHELPERS_H
