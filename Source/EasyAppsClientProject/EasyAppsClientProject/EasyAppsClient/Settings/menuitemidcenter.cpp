#include "menuitemidcenter.h"

MenuItemIdCenter::MenuItemIdCenter()
{
}

QString MenuItemIdCenter::myAppsMenuId()
{
    static QString r = QString("MyApps");
    return r;
}


//
// store stack

QString MenuItemIdCenter::storeMenuId()
{
    static QString r = QString("StoreMenuItemId");
    return r;
}

QString MenuItemIdCenter::storeSearchLineSubMenuItemId()
{
    static QString r = QString("StoreSearchLineSubMenuItemId");
    return r;
}

//QString MenuItemIdCenter::appStoreSearchResultPageMenuId()
//{
//    static QString r = QString("AppStoreSearchResultPageMenuId");
//    return r;
//}

QString MenuItemIdCenter::storeMorePageMenuItem()
{
    static QString r = QString("StoreMorePage");
    return r;
}

QString MenuItemIdCenter::appDetailPageMenuItem()
{
    static QString r = QString("AppDetailPage");
    return r;
}

QString MenuItemIdCenter::licencePresenterPageMenuId()
{
    static QString r = QString("LicencePresenter");
    return r;
}

QString MenuItemIdCenter::appwhirrLicencePresenterPageMenuId()
{
    static QString r = QString("AppwhirrLicencePresenterPageMenuId");
    return r;
}

//
// options stack

QString MenuItemIdCenter::optionsMenuId()
{
    static QString r = QString("Options");
    return r;
}

QString MenuItemIdCenter::helpMenuId()
{
    static QString r = QString("Help");
    return r;
}

QString MenuItemIdCenter::suggestAnAppMenuId()
{
    static QString r = QString("SuggestAnApp");
    return r;
}

QString MenuItemIdCenter::feedbackPageMenuId()
{
    static QString r = QString("Feedback");
    return r;
}

QString MenuItemIdCenter::whatsNewPageMenuId()
{
    static QString r = QString("WhatsNew");
    return r;
}
