#ifndef MENUITEMIDCENTER_H
#define MENUITEMIDCENTER_H

#include <QString>

class MenuItemIdCenter
{
private:
    MenuItemIdCenter();

public:

    // my apps stack
    static QString myAppsMenuId();

    // store stack
    static QString storeMenuId();
    static QString storeSearchLineSubMenuItemId();
    static QString storeMorePageMenuItem();
    static QString appStoreSearchResultPageMenuId();
    static QString appDetailPageMenuItem();
    static QString licencePresenterPageMenuId();
    static QString appwhirrLicencePresenterPageMenuId();

    // options stack
    static QString optionsMenuId();
    static QString helpMenuId();
    static QString suggestAnAppMenuId();
    static QString feedbackPageMenuId();
    static QString whatsNewPageMenuId();
};

#endif // MENUITEMIDCENTER_H
