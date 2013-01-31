#include "awshortcuthandler.h"

#include "Common/DesktopShortcutCreator/desktopshortcutcreator.h"
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"
#include "Application/Delegates/urlschemehandlerdelegate.h"
#include "Application/Utils/localapppathhelper.h"

namespace AW {
namespace Client {
namespace Utils {

AWShortcutHandler::AWShortcutHandler(QObject *parent) :
    QObject(parent)
{
}

bool AWShortcutHandler::createSchemeBasedDesktopShortcutForApp(ApplicationDescriptionModel appDescrModel)
{
    QString iconPath;
    QString pathToSaveTheShortcut = PathHelper::getLocalDesktopPath();

    #ifdef Q_WS_WIN
    pathToSaveTheShortcut = PathHelper::combineAndCleanPathes(pathToSaveTheShortcut, appDescrModel.getDynamicDescription_ReadOnly()->getAppName() + ".lnk");
    iconPath = AW::Client::Utils::LocalAppPathHelper::getAppExecutableFullPath(appDescrModel);
//    iconPath = PathHelper::combineAndCleanPathes(appDescrModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescrModel.getStaticDescription_ReadOnly()->getLargeIconPath());
    iconPath = PathHelper::makePathPlatformDependent(iconPath);
//    iconPath = PathHelper::quotePath();
#endif
#ifdef Q_WS_MAC
    pathToSaveTheShortcut = PathHelper::combineAndCleanPathes(pathToSaveTheShortcut, appDescrModel.getDynamicDescription_ReadOnly()->getAppName() + ".app");
    iconPath = AW::Client::Utils::LocalAppPathHelper::getAppExecutableFullPath(appDescrModel);
#endif

    return DesktopShortcutCreator::createShortcut(pathToSaveTheShortcut, UrlSchemeHandlerDelegate::createSchemeUrlForRunApp(appDescrModel.getDynamicDescription_ReadOnly()->getAppId()), iconPath, QString("Run %1").arg(appDescrModel.getDynamicDescription_ReadOnly()->getAppName()), false);
}

#ifdef Q_OS_WIN
bool AWShortcutHandler::createSchemeBasedStartMenuShortcutForApp(ApplicationDescriptionModel appDescrModel)
{
    QString iconPath;

    QString pathToSaveTheShortcut = PathHelper::combineAndCleanPathes(PathHelper::getWinSpecStartMenuPath(), appDescrModel.getDynamicDescription_ReadOnly()->getAppName() + ".lnk");
    iconPath = AW::Client::Utils::LocalAppPathHelper::getAppExecutableFullPath(appDescrModel);
//    iconPath = PathHelper::combineAndCleanPathes(appDescrModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), appDescrModel.getStaticDescription_ReadOnly()->getLargeIconPath());
    iconPath = PathHelper::makePathPlatformDependent(iconPath);
//    iconPath = PathHelper::quotePath();

    return DesktopShortcutCreator::createShortcut(pathToSaveTheShortcut, UrlSchemeHandlerDelegate::createSchemeUrlForRunApp(appDescrModel.getDynamicDescription_ReadOnly()->getAppId()), iconPath, QString("Run %1").arg(appDescrModel.getDynamicDescription_ReadOnly()->getAppName()), false);
}

bool AWShortcutHandler::removeSchemeBasedStartMenuShortcutForApp(ApplicationDescriptionModel appDescrModel)
{
    QString pathToSaveTheShortcut = PathHelper::combineAndCleanPathes(PathHelper::getWinSpecStartMenuPath(), appDescrModel.getDynamicDescription_ReadOnly()->getAppName() + ".lnk");

    return QFile(pathToSaveTheShortcut).remove();
}
#endif

bool AWShortcutHandler::createDesktopShortcutForAppWhirrClient()
{
    QString pathToSaveTheShortcut = PathHelper::getLocalDesktopPath();
#ifdef Q_WS_WIN
    pathToSaveTheShortcut = PathHelper::combineAndCleanPathes(pathToSaveTheShortcut, "AppWhirr.lnk");
#endif
#ifdef Q_WS_MAC
    // Nothing to do here
#endif
    return DesktopShortcutCreator::createShortcut(pathToSaveTheShortcut, AppSpecificPathHelper::getClientExecutablePath(), QString(), "Start AppWhirr client", true);
}


} // namespace Utils
} // namespace Client
} // namespace AW
