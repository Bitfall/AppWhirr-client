#include "startthisapplicationevent.h"

StartThisApplicationEvent::StartThisApplicationEvent(QString applicationPath,
                                                     QString applicationWorkingDirectory,
                                                     QString applicationTitle,
                                                     QString relatedFilter)
        : QEvent(getCustomEventType()),
          applicationPath(applicationPath),
          applicationWorkingDirectory(applicationWorkingDirectory),
          applicationTitle(applicationTitle),
          relatedFilter(relatedFilter)
{
}

QString StartThisApplicationEvent::getApplicationPath() const
{
    return this->applicationPath;
}

QString StartThisApplicationEvent::getApplicationWorkingDirectory() const {
    return this->applicationWorkingDirectory;
}

QString StartThisApplicationEvent::getApplicationTitle() const
{
    return this->applicationTitle;
}

QString StartThisApplicationEvent::getRelatedFilter() const
{
    return this->relatedFilter;
}
