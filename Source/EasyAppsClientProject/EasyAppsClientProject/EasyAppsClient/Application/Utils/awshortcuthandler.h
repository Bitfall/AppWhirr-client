#ifndef APPWHIRR_CLIENT_UTILS_AWSHORTCUTHANDLER_H
#define APPWHIRR_CLIENT_UTILS_AWSHORTCUTHANDLER_H

#include <QObject>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

namespace AW {
namespace Client {
namespace Utils {

class AWShortcutHandler : public QObject
{
    Q_OBJECT
public:
    explicit AWShortcutHandler(QObject *parent = 0);
    
    /*! Creates a scheme based desktop shortcut for the given app
    */
    static bool createSchemeBasedDesktopShortcutForApp(ApplicationDescriptionModel appDescrModel);

#ifdef Q_OS_WIN
    /*! [win only]
    */
    static bool createSchemeBasedStartMenuShortcutForApp(ApplicationDescriptionModel appDescrModel);
    static bool removeSchemeBasedStartMenuShortcutForApp(ApplicationDescriptionModel appDescrModel);
#endif

    static bool createDesktopShortcutForAppWhirrClient();
};

} // namespace Utils
} // namespace Client
} // namespace AW

#endif // APPWHIRR_CLIENT_UTILS_AWSHORTCUTHANDLER_H
