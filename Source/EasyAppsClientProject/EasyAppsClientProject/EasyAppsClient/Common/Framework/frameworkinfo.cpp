#include "frameworkinfo.h"

FrameworkInfo::FrameworkInfo()
{
}

FrameworkInfo::FrameworkInfo(QString frameworkId, QString frameworkPrintableName) :
    _frameworkId(frameworkId),
    _frameworkPrintableName(frameworkPrintableName)
{
}

QString FrameworkInfo::getFrameworkId() const
{
    return _frameworkId;
}

QString FrameworkInfo::getFrameworkPrintableName() const
{
    return _frameworkPrintableName;
}
