#include "frameworkcheckerinfo.h"

FrameworkCheckerInfo::FrameworkCheckerInfo()
{
}

FrameworkCheckerInfo::FrameworkCheckerInfo(QString frameworkCheckerId, QString relatedFrameworkName, QString frameworkCheckerPath, QString frameworkCheckerUpdateTimestamp) :
    _frameworkCheckerId(frameworkCheckerId),
    _relatedFrameworkName(relatedFrameworkName),
    _frameworkCheckerPath(frameworkCheckerPath),
    _frameworkCheckerUpdateTimestamp(frameworkCheckerUpdateTimestamp)
{
}

QString FrameworkCheckerInfo::getFrameworkCheckerId() const
{
    return _frameworkCheckerId;
}

void FrameworkCheckerInfo::setFrameworkCheckerId(QString value)
{
    _frameworkCheckerId = value;
}

QString FrameworkCheckerInfo::getRelatedFrameworkName() const
{
    return _relatedFrameworkName;
}

void FrameworkCheckerInfo::setRelatedFrameworkName(QString value)
{
    _relatedFrameworkName = value;
}

QString FrameworkCheckerInfo::getFrameworkCheckerPath() const
{
    return _frameworkCheckerPath;
}

void FrameworkCheckerInfo::setFrameworkCheckerPath(QString value)
{
    _frameworkCheckerPath = value;
}

QString FrameworkCheckerInfo::getFrameworkCheckerUpdateTimestamp() const
{
    return _frameworkCheckerUpdateTimestamp;
}

void FrameworkCheckerInfo::setFrameworkCheckerUpdateTimestamp(QString value)
{
    _frameworkCheckerUpdateTimestamp = value;
}
