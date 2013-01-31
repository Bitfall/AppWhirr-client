#include "pageidcenter.h"

PageIdCenter::PageIdCenter()
{
}

QString PageIdCenter::myAppsPageId()
{
    QString r("MyAppsPage");
    return r;
}

QString PageIdCenter::storePageId()
{
    QString r("StorePage");
    return r;
}

QString PageIdCenter::optionsPageId()
{
    QString r("OptionsPage");
    return r;
}

QString PageIdCenter::feedbackPageId()
{
    QString r("FeedbackPage");
    return r;
}

QString PageIdCenter::suggestAnAppOptionsSubPageId()
{
    return QString("Options-SuggestAnAppPage");
}

QString PageIdCenter::helpPageId()
{
    QString r("HelpPage");
    return r;
}

QString PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId()
{
    QString r("ClientUpdateAndInitialLocalAppDatabaseLoaderPageId");
    return r;
}
