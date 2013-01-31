#ifndef WEBSITELISTDATABASEMANAGER_H
#define WEBSITELISTDATABASEMANAGER_H

#include "Common/Database/basedatabase.h"
#include "websitelistdatabaseitemmodel.h"

#include <QList>

class WebsiteListDatabaseManager : public BaseDatabase
{
    Q_OBJECT
public:
    explicit WebsiteListDatabaseManager(QObject *parent = 0);

    QList<WebsiteListDatabaseItemModel> getEveryItem();
    bool storeItems(QList<WebsiteListDatabaseItemModel> items);
    bool addNewItem(WebsiteListDatabaseItemModel newItem);

    bool importDatabase(QString importPath);
    bool exportDatabase(QString exportPath);

protected:
    QString getDatabasePath();
    bool copyDatabaseFromResourceToPath(QString path) {return false;} // will be created in code
    bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill);

private:
    QString _databasePath;
};

#endif // WEBSITELISTDATABASEMANAGER_H
