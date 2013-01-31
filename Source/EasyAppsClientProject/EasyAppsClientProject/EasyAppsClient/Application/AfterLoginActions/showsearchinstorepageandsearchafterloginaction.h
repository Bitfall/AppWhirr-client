#ifndef SHOWSEARCHINSTOREPAGEANDSEARCHAFTERLOGINACTION_H
#define SHOWSEARCHINSTOREPAGEANDSEARCHAFTERLOGINACTION_H

#include "afterloginactioninterface.h"

class ShowSearchInStorePageAndSearchAfterLoginAction : public AfterLoginActionInterface
{
public:
    explicit ShowSearchInStorePageAndSearchAfterLoginAction(QString searchFilterText) : _searchFilterText(searchFilterText) {}

    QString getTypeId() const { return "ShowSearchInStorePageAndSearchAfterLoginAction"; }

    QString getSearchFilterText() const { return _searchFilterText; }

private:
    QString _searchFilterText;
};

#endif // SHOWSEARCHINSTOREPAGEANDSEARCHAFTERLOGINACTION_H
