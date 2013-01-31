#ifndef DATABASETABLESCHEMEDESCRIPTOR_H
#define DATABASETABLESCHEMEDESCRIPTOR_H

#include <QList>
#include <QString>

//
class DatabaseTableSchemeDescriptor
{
public:
    //
    // a wrapper-model around SQLite's: PRAGMA TABLE_INFO(tablename)
    class DatabaseTableSchemeDescriptorItem
    {
    public:
        explicit DatabaseTableSchemeDescriptorItem(int cid, QString name, QString type) : _cid(cid), _name(name), _type(type) {}

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

public:
    explicit DatabaseTableSchemeDescriptor() {}

    void addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem item);
    QList<DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem> getItems() const;

    bool isEmpty() const;

    bool isContainsItemWithName(QString name) const;
    void removeOneByColumnName(QString name);
    // will return a string like this:
    //  "platformFlags TEXT, grabbedIconPath TEXT, isHidden NUMERIC"
    QString getTableColumnNameAndTypeListString() const;
    //
    // will return a string like this:
    //  "platformFlags, grabbedIconPath, isHidden"
    QString getTableColumnNameListString() const;

private:
    QList<DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem> _items;
};

#endif // DATABASETABLESCHEMEDESCRIPTOR_H
