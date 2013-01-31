#ifndef SPECIALLOCALAPPDATABASEOPERATIONIDS_H
#define SPECIALLOCALAPPDATABASEOPERATIONIDS_H

#include <QString>

class SpecialLocalAppDatabaseOperationIds
{
private:
    SpecialLocalAppDatabaseOperationIds();

public:
    static QString getInitialWholeAppDatabaseQueryForUserQueryOperationId();
    static QString getStorePageReloadQueryOperationId();
};

#endif // SPECIALLOCALAPPDATABASEOPERATIONIDS_H
