#include "awapplicationrunner.h"

#include <QApplication>

#include "Common/Event/startthisapplicationevent.h"
#include "Application/Utils/localapppathhelper.h"

namespace AW {
namespace Client {
namespace Utils {

AWApplicationRunner::AWApplicationRunner(QObject *parent) :
    QObject(parent)
{
}

void AWApplicationRunner::postRunThisAppEvent(ApplicationDescriptionModel appDescrModel, QString filterText)
{
    QString executableAbsolutePath = AW::Client::Utils::LocalAppPathHelper::getAppExecutableFullPath(appDescrModel);
    QString absoluteWorkingDir = AW::Client::Utils::LocalAppPathHelper::getAppExecutableWorkingDirFullPath(appDescrModel);

    QApplication::postEvent(QApplication::instance(), new StartThisApplicationEvent(executableAbsolutePath, absoluteWorkingDir, appDescrModel.getDynamicDescription_ReadOnly()->getAppName(), filterText));
}

} // namespace Utils
} // namespace Client
} // namespace AW
