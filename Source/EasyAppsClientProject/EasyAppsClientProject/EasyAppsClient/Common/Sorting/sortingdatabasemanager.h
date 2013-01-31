#ifndef SORTINGDATABASEMANAGER_H
#define SORTINGDATABASEMANAGER_H

#include "Common/Database/basedatabase.h"

class SortingDatabaseManager : public BaseDatabase
{
    Q_OBJECT
public:
    explicit SortingDatabaseManager(QString databasePath, QObject *parent = 0);

public:
    bool setValueForKey(QString key, QString value);

    //
    //
    bool getValueForKey(QString key, QString &outValue);

    bool getAllFilterKeyValuePairs(QMap<QString,QString> &outKeyValueMap);

protected:
    QString getDatabasePath();
    bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill);

private:
    QString _databasePath;
};

#endif // SORTINGDATABASEMANAGER_H
