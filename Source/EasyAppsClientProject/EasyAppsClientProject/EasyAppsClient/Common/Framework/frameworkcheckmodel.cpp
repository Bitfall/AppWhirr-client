#include "frameworkcheckmodel.h"

FrameworkCheckModel::FrameworkCheckModel()
{
}

FrameworkCheckModel::FrameworkCheckModel(FrameworkInfo frameworkInfo, QString frameworkCheckerAppPath, QString frameworkCheckerUpdateTimestamp, QString frameworkResultFileDirPath, QString frameworkInstallerPath, QStringList optionalFrameworkCheckerArgs) :
    _frameworkInfo(frameworkInfo),
    _frameworkCheckerAppPath(frameworkCheckerAppPath),
    _frameworkCheckerUpdateTimestamp(frameworkCheckerUpdateTimestamp),
    _frameworkResultFileDirPath(frameworkResultFileDirPath),
    _frameworkInstallerPath(frameworkInstallerPath),
    _optionalFrameworkCheckerArgs(optionalFrameworkCheckerArgs)
{
}

FrameworkInfo FrameworkCheckModel::getFrameworkInfo() const
{
    return _frameworkInfo;
}

QString FrameworkCheckModel::getFrameworkCheckerAppPath() const {
    return _frameworkCheckerAppPath;
}

QString FrameworkCheckModel::getFrameworkCheckerUpdateTimestamp() const
{
    return _frameworkCheckerUpdateTimestamp;
}

QString FrameworkCheckModel::getFrameworkResultFileDirPath() const
{
    return _frameworkResultFileDirPath;
}

QString FrameworkCheckModel::getFrameworkInstallerPath() const
{
    return _frameworkInstallerPath;
}

QStringList FrameworkCheckModel::getOptionalFrameworkCheckerArgs() const
{
    return _optionalFrameworkCheckerArgs;
}

void FrameworkCheckModel::_setFrameworkCheckerAppPath(QString value)
{
    _frameworkCheckerAppPath = value;
}

void FrameworkCheckModel::_setFrameworkCheckerUpdateTimestamp(QString value)
{
    _frameworkCheckerUpdateTimestamp = value;
}

void FrameworkCheckModel::_setFrameworkInstallerPath(QString value)
{
    _frameworkInstallerPath = value;
}

void FrameworkCheckModel::replaceOptionalFrameworkCheckerArg(QStringList args)
{
    _optionalFrameworkCheckerArgs = args;
}

void FrameworkCheckModel::addOptionalFrameworkCheckerArg(QString arg)
{
    _optionalFrameworkCheckerArgs.append(arg);
}

void FrameworkCheckModel::addOptionalFrameworkCheckerArgs(QStringList argsToAdd)
{
    _optionalFrameworkCheckerArgs.append(argsToAdd);
}

void FrameworkCheckModel::clearOptionalFrameworkCheckerArgs()
{
    _optionalFrameworkCheckerArgs.clear();
}
