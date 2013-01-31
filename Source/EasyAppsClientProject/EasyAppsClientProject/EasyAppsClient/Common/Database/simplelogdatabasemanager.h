#ifndef SIMPLELOGDATABASEMANAGER_H
#define SIMPLELOGDATABASEMANAGER_H

#include "basedatabase.h"

class SimpleLogDatabaseManager
{
private:
    class SimpleLogDatabase : BaseDatabase {
    public:
        SimpleLogDatabase(QString logFilePath, QString baseConnectionName);

        void writeToLog(QString logText);

    protected:
        QString getDatabasePath();
        bool copyDatabaseFromResourceToPath(QString path) {return false;}
        bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill);

    private:
        QString _logFilePath;
    };

private:
    explicit SimpleLogDatabaseManager() {}

public:
    //
    // logFileName is the filename, not the path!!
    //  the log file will be created in the Logs directory
    static void writeToLog(QString logFileName, QString logText);
};

#endif // SIMPLELOGDATABASEMANAGER_H
