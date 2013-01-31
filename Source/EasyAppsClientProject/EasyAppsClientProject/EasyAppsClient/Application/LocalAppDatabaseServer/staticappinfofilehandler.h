#ifndef STATICAPPINFOFILEHANDLER_H
#define STATICAPPINFOFILEHANDLER_H

#include "Common/Database/basedatabase.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class StaticAppInfoFileHandler : public BaseDatabase
{
    Q_OBJECT
public:
    explicit StaticAppInfoFileHandler(QString appInfoDatabaseFilePath, QObject *parent = 0);

    bool parseStaticAppInfoToApplicationDescriptionModel(ApplicationDescriptionModel &refAppDescriptionModel);

    bool updateStaticInfoByApplicationDescriptionModel(ApplicationDescriptionModel appDescriptionModel);

protected:
    QString getDatabasePath();

    // it won't create database, only use an existing one
    bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill) { return false; }
    bool copyDatabaseFromResourceToPath(QString path) {return false;}

private:
    QString _appInfoDatabaseFilePath;
};

#endif // STATICAPPINFOFILEHANDLER_H
