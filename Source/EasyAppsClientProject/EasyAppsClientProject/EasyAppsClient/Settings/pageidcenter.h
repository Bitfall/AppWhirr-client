#ifndef PAGEIDCENTER_H
#define PAGEIDCENTER_H

#include <QString>

class PageIdCenter
{
private:
    PageIdCenter();

public:
    static QString myAppsPageId();
    static QString storePageId();
    static QString optionsPageId();
    static QString feedbackPageId();
    static QString suggestAnAppOptionsSubPageId();
    static QString helpPageId();
    static QString clientUpdateAndInitialLocalAppDatabaseLoaderPageId();
};

#endif // PAGEIDCENTER_H
