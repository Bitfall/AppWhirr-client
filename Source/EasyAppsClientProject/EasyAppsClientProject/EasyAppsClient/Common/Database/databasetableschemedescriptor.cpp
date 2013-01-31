#include "databasetableschemedescriptor.h"

void DatabaseTableSchemeDescriptor::addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem item) {
    _items.append(item);
}

QList<DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem> DatabaseTableSchemeDescriptor::getItems() const {
    return _items;
}

bool DatabaseTableSchemeDescriptor::isEmpty() const {
    return _items.isEmpty();
}

bool DatabaseTableSchemeDescriptor::isContainsItemWithName(QString name) const {
    int itemCnt = _items.size();
    bool isFound = false;
    for(int i = 0; i < itemCnt && !isFound; i++) {
        if(_items[i].getName() == name) {
            isFound = true;
        }
    }

    return isFound;
}

void DatabaseTableSchemeDescriptor::removeOneByColumnName(QString name) {
    int itemCnt = _items.size();
    int foundIdx = -1;
    for(int i = 0; i < itemCnt && foundIdx < 0; i++) {
        if(_items[i].getName() == name) {
            foundIdx = i;
        }
    }

    if(foundIdx >= 0) {
        // remove it
        this->_items.removeAt(foundIdx);
    }
}

QString DatabaseTableSchemeDescriptor::getTableColumnNameAndTypeListString() const
{
    QString result;

    for(int i = 0; i < this->_items.size(); i++) {
        if(i > 0) {
            result += ", ";
        }
        result += this->_items[i].getName() + " " + this->_items[i].getType();
    }

    return result;
}

QString DatabaseTableSchemeDescriptor::getTableColumnNameListString() const
{
    QString result;

    for(int i = 0; i < this->_items.size(); i++) {
        if(i > 0) {
            result += ", ";
        }
        result += this->_items[i].getName();
    }

    return result;
}
