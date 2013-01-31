#ifndef LOCALAPPDATABASEWORKPERFORMER_H
#define LOCALAPPDATABASEWORKPERFORMER_H

#include "Common/Database/databaseworkperformerbase.h"

class LocalAppDatabaseWorkPerformer : public DatabaseWorkPerformerBase
{
public:
    explicit LocalAppDatabaseWorkPerformer(QString databasePath);

protected:
    bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill);

    DatabaseTableSchemeDescriptor _getRequiredSchemeDescriptionForTable(QString tableName);

private:
    DatabaseTableSchemeDescriptor _dynamicAppInfoTableScheme;
    DatabaseTableSchemeDescriptor _staticAppInfoTableScheme;
};

#endif // LOCALAPPDATABASEWORKPERFORMER_H
