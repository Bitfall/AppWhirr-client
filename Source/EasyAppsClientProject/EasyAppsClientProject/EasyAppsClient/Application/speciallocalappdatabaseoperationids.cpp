#include "speciallocalappdatabaseoperationids.h"

SpecialLocalAppDatabaseOperationIds::SpecialLocalAppDatabaseOperationIds()
{
}

QString SpecialLocalAppDatabaseOperationIds::getInitialWholeAppDatabaseQueryForUserQueryOperationId() {
    return "initial-whole-app-database-query-for-user";
}

QString SpecialLocalAppDatabaseOperationIds::getStorePageReloadQueryOperationId() {
    return "StoreMainPage-Reload";
}
