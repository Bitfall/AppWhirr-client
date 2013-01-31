#include "websitelistdatabasemanager.h"

// sql
#include <QSqlQuery>
#include <QVariant>

// import / export file handling
#include <QFile>
#include <QTextStream>

#include <QStringList>

// config
#define IMPORT_EXPORT_DATA_SEPARATOR            "||---||"
#define IMPORT_EXPORT_DATA_ITEM_SEPARATOR       "||@@@||"

WebsiteListDatabaseManager::WebsiteListDatabaseManager(QObject *parent) :
    BaseDatabase("WebsiteListDatabaseManager", parent)
{
}

QString WebsiteListDatabaseManager::getDatabasePath()
{
//    return this->_databasePath;
    return "TmpUpdateCheckDatabase.db";
//    return "E:\\AW_Dev\\UsefulScriptsAndTools\\AppDatabaseUpdateChecker\\AppDatabaseUpdateChecker-build-desktop-Qt_4_7_4_for_Desktop_-_MSVC2008__Qt_SDK__Debug\\baseAppUpdateDatabase.db";
}

bool WebsiteListDatabaseManager::fillDatabaseWithBasicData(QSqlDatabase databaseToFill)
{
    QSqlQuery query(databaseToFill);
    query.prepare("CREATE TABLE websites (appname TEXT UNIQUE, url TEXT, downloadUrl TEXT, content TEXT, timestamp TEXT, timestampAsText TEXT, checkSectionBegin TEXT, checkSectionEnd TEXT, note TEXT)");
    if(!query.exec()) {
        this->logQueryExecutionFailedWithLastError(databaseToFill);
        return false;
    }

    return true;
}

QList<WebsiteListDatabaseItemModel> WebsiteListDatabaseManager::getEveryItem()
{
    QList<WebsiteListDatabaseItemModel> items;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        QSqlQuery query(db);
        QString queryString = QString("SELECT appname, url, downloadUrl, content, timestamp, timestampAsText, checkSectionBegin, checkSectionEnd, note FROM websites");
        query.prepare(queryString);
        if(!query.exec()) {
            this->logQueryExecutionFailedWithLastError(db);
        }
        else {
            while(query.next()) {
                items.append(WebsiteListDatabaseItemModel(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toString(), query.value(4).toString(), query.value(5).toString(), query.value(6).toString(), query.value(7).toString(), query.value(8).toString()));
            }
        }
    }

    this->closeDatabaseConnection();
    return items;
}

bool WebsiteListDatabaseManager::addNewItem(WebsiteListDatabaseItemModel newItem)
{
    QList<WebsiteListDatabaseItemModel> singleItemList;
    singleItemList.append(newItem);

    return this->storeItems(singleItemList);
}

bool WebsiteListDatabaseManager::storeItems(QList<WebsiteListDatabaseItemModel> items)
{
    bool isEverythingSucceeded = true;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        QString baseQueryString = QString("INSERT OR REPLACE INTO websites (appname, url, downloadUrl, content, timestamp, timestampAsText, checkSectionBegin, checkSectionEnd, note) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

        int itemCnt = items.size();
        for(int i = 0; i < itemCnt; i++)
        {
            WebsiteListDatabaseItemModel currItem = items[i];
            QSqlQuery query(db);
            query.prepare(baseQueryString);
            query.bindValue(0, currItem.getAppName());
            query.bindValue(1, currItem.getUrl());
            query.bindValue(2, currItem.getDownloadUrl());
            query.bindValue(3, currItem.getContent());
            query.bindValue(4, currItem.getTimestamp());
            query.bindValue(5, currItem.getTimestampAsText());
            query.bindValue(6, currItem.getCheckSectionBegin());
            query.bindValue(7, currItem.getCheckSectionEnd());
            query.bindValue(8, currItem.getNote());
            if(!query.exec()) {
                this->logQueryExecutionFailedWithLastError(db);
                isEverythingSucceeded = false;
            }
        }
    }

    this->closeDatabaseConnection();
    return isEverythingSucceeded;
}

bool WebsiteListDatabaseManager::importDatabase(QString importPath)
{
    QFile file(importPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        WLog("Cannot open the import file: ") << importPath;
        return false;
    }


    QList<WebsiteListDatabaseItemModel> importedItems;

    QTextStream in(&file);
    QString contentOfImportFile = in.readAll();

    //
    // split by item separator
    QStringList itemContentStrings = contentOfImportFile.split(IMPORT_EXPORT_DATA_ITEM_SEPARATOR, QString::SkipEmptyParts);
    for(int i = 0; i < itemContentStrings.size(); i++)
    {
        // split items to data-parts
        QString currItemContentString = itemContentStrings[i];
        QStringList itemPartStrings = currItemContentString.split(IMPORT_EXPORT_DATA_SEPARATOR, QString::KeepEmptyParts);
        if(itemPartStrings.isEmpty()) {
            WLog("Invalid (empty) item.");
        }
        else if(itemPartStrings.size() != 9) {
            WLog("Invalid item - should contain 9 data-parts.");
        }
        else {
            // read it
            QString appName = itemPartStrings[0];
            QString timestampAsText = itemPartStrings[1];
            QString timestamp = itemPartStrings[2];
            QString url = itemPartStrings[3];
            QString downloadUrl = itemPartStrings[4];
            QString checkSectionBegin = itemPartStrings[5];
            QString checkSectionEnd = itemPartStrings[6];
            QString note = itemPartStrings[7];
            QString content = itemPartStrings[8];

            importedItems.append(WebsiteListDatabaseItemModel(appName, url, downloadUrl, content, timestamp, timestampAsText, checkSectionBegin, checkSectionEnd, note));
        }
    }

    if(importedItems.isEmpty()) {
        WLog("No items found in the imported file!");
        return false;
    }

    return this->storeItems(importedItems);
}

bool WebsiteListDatabaseManager::exportDatabase(QString exportPath)
{
    QList<WebsiteListDatabaseItemModel> items = this->getEveryItem();

    if(items.isEmpty()) {
        WLog("No items found to export.");
        return false;
    }

    QFile file(exportPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        WLog("Cannot open the export file: ") << exportPath;
        return false;
    }

    QTextStream out(&file);
//    out << "The magic number is: " << 49 << "\n";

    for(int i = 0; i < items.size(); i++)
    {
        WebsiteListDatabaseItemModel currItem = items[i];
        out << currItem.getAppName() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getTimestampAsText() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getTimestamp() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getUrl() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getDownloadUrl() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getCheckSectionBegin() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getCheckSectionEnd() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getNote() << IMPORT_EXPORT_DATA_SEPARATOR << currItem.getContent() << IMPORT_EXPORT_DATA_ITEM_SEPARATOR;
    }

    file.close();

    return true;
}
