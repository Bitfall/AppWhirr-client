#ifndef FRAMEWORKCHECKERDATABASE_H
#define FRAMEWORKCHECKERDATABASE_H

#include "Common/Database/basedatabase.h"

#include "Application/Frameworks/frameworkcheckerinfo.h"

/*! Manages the framework-checker database.

    This database contains information about the downloaded framework checkers.
*/
class FrameworkCheckerDatabase : public BaseDatabase
{
    Q_OBJECT
public:
    explicit FrameworkCheckerDatabase(QString databasePath, QObject *parent = 0);

public:
    /*! Adds framework-checker information to the databse.

        @return true if the query can be executed, false if not.
    */
    bool addFrameworkChecker(FrameworkCheckerInfo frameworkCheckerInfo);

    /*! Gets framework-checker information from the databse.

        @return true if the query can be executed, false if not.
    */
    bool getFrameworkChecker(QString frameworkCheckerId, FrameworkCheckerInfo &outValue);

    // ----------------
    // --- internal ---
protected:
    QString getDatabasePath();
    bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill);

private:
    QString _databasePath;
};

#endif // FRAMEWORKCHECKERDATABASE_H
