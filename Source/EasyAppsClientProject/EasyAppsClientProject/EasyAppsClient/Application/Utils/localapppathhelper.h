#ifndef AW_CLIENT_UTILS_LOCALAPPPATHHELPER_H
#define AW_CLIENT_UTILS_LOCALAPPPATHHELPER_H

#include <QObject>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

namespace AW {
namespace Client {
namespace Utils {

class LocalAppPathHelper : public QObject
{
    Q_OBJECT
public:
    explicit LocalAppPathHelper(QObject *parent = 0);
    
    /*! Installed-path + executable-path
    */
    static QString getAppExecutableFullPath(ApplicationDescriptionModel appDescriptionModel);

    /*! Installed-path + executable-working-dir-path
    */
    static QString getAppExecutableWorkingDirFullPath(ApplicationDescriptionModel appDescriptionModel);
};

} // namespace Utils
} // namespace Client
} // namespace AW

#endif // AW_CLIENT_UTILS_LOCALAPPPATHHELPER_H
