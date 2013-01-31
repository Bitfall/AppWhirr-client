#include "localapppathhelper.h"

#include "Common/Helpers/pathhelper.h"

namespace AW {
namespace Client {
namespace Utils {

LocalAppPathHelper::LocalAppPathHelper(QObject *parent) :
    QObject(parent)
{
}

QString LocalAppPathHelper::getAppExecutableFullPath(ApplicationDescriptionModel appDescriptionModel)
{
    return PathHelper::combineAndCleanPathes(appDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescriptionModel.getStaticDescription_ReadOnly()->getExecutablePath());
}

QString LocalAppPathHelper::getAppExecutableWorkingDirFullPath(ApplicationDescriptionModel appDescriptionModel)
{
    return PathHelper::combineAndCleanPathes(appDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescriptionModel.getStaticDescription_ReadOnly()->getExecutableWorkingDirectory());
}

} // namespace Utils
} // namespace Client
} // namespace AW
