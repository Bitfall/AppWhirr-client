#ifndef BASEDATABASE_H
#define BASEDATABASE_H

#include <QObject>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
class QSqlError;
class QString;
QT_END_NAMESPACE

//
// one of the virtual database-creation methods have to be implemented (if none of them implemented openDatabase() will always return false)
//
// note: you have to call openDatabaseConnectionAndReturnDatabase() before every (batch of) operation(s) you want to make (typically call this at the beginning of a method which will use the database) and you have to call closeDatabaseConnection() when you finished with the operations
//  the only exception is the fillDatabaseWithBasicData method, which gets the open database as a parameter, and have to use it, and is have to NOT call either openDatabaseConnectionAndReturnDatabase() or closeDatabaseConnection()
//  warning: you HAVE TO CLOSE the database even if you want to return earlier than the end of the method (e.g. if there were some errors)! don't forget to close the database!
//
// For usage informations check the Qt doc/help of the QSqlDatabase class, and use it that way. This base class has only some helper methods (basic query creation, loggin, ...) and unified database and connection opening/closing methods, but the QSqlDatabase and the query execution is done "the Qt way"
//
class BaseDatabase : public QObject
{
    Q_OBJECT

protected:

    //
    // a wrapper-model around SQLite's: PRAGMA TABLE_INFO(tablename)
    class DatabaseSchemeDescriptionItem {
    public:
        explicit DatabaseSchemeDescriptionItem(int cid, QString name, QString type) : _cid(cid), _name(name), _type(type) {}

        // cid = Column ID
        int getCid() const { return _cid; }
        // name = Column Name
        QString getName() const { return _name; }
        QString getType() const { return _type; }

    private:
        int _cid;
        QString _name;
        QString _type;
    };

    class DatabaseSchemeDescription {
    public:
        explicit DatabaseSchemeDescription() : _isValid(false) {}

        void addItem(DatabaseSchemeDescriptionItem item) { _items.append(item); }
        QList<DatabaseSchemeDescriptionItem> getItems() const { return _items; }

        //
        // isValid means whether the scheme-description info was successfully retrieved or not
        void setIsValid(bool value) { this->_isValid = value; }
        bool getIsValid() const { return this->_isValid; }

        bool isEmpty() const { return (_items.size() <= 0); }

        bool isContainsItemWithName(QString name) {
            int itemCnt = _items.size();
            bool isFound = false;
            for(int i = 0; i < itemCnt && !isFound; i++) {
                if(_items[i].getName() == name) {
                    isFound;
                }
            }

            return isFound;
        }

    private:
        QList<DatabaseSchemeDescriptionItem> _items;

        bool _isValid;
    };

public:
    explicit BaseDatabase(const QString& connectionName, QObject *parent = 0);
    virtual ~BaseDatabase();

    //
    // this will completely delete the database, you won't be able to restore it
    bool _completelyDeleteDatabase();

protected:
    QSqlDatabase openDatabaseConnectionAndReturnDatabase();
    void closeDatabaseConnection();

    QSqlError getLastError(QSqlDatabase db);

    DatabaseSchemeDescription _getCurrentDatabaseSchemeDescriptionForTable(QString tableName);


    // --- Implement these!

    //
    // you have to implement this and return the database's path, where it will be stored
    virtual QString getDatabasePath() = 0;
    //
    // return whether it's succeeded
    virtual bool copyDatabaseFromResourceToPath(QString path) {return false;}
    //
    // return whether it's succeeded
    virtual bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill) {return false;}

//    QSqlDatabase getDatabase();


    // --- Logging methods

    void logQueryExecutionFailedWithLastError(QSqlDatabase db);

    // --- Query creation helper methods

    //
    // tableName has to be specified
    // valueColumnName has to be specified
    QString createSingleColumnSelectQueryString(QString tableName, QString valueColumnName, QString keyColumnName = QString(), QString keyFilterText = QString(), int limitResultToNumber = -1);

private:
//    QSqlDatabase            db;
    QString                 dbConnectionName;
};

#endif // BASEDATABASE_H
