#ifndef APPWHIRR_CLIENT_UTILS_AWAPPLICATIONRUNNER_H
#define APPWHIRR_CLIENT_UTILS_AWAPPLICATIONRUNNER_H

#include <QObject>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

namespace AW {
namespace Client {
namespace Utils {

class AWApplicationRunner : public QObject
{
    Q_OBJECT
public:
    explicit AWApplicationRunner(QObject *parent = 0);
    
    /*! Posts a \a StartThisApplicationEvent event for the \a QApplication

        @param filterText : if provided will be used for activity-tracking - AppWhirr tracks these user provided filter-texts (search texts) and provide intelligent sorting based on user activity - an app with a tracked filterText may be presented before other apps in related search results
    */
    static void postRunThisAppEvent(ApplicationDescriptionModel appDescrModel, QString filterText = QString());
    
};

} // namespace Utils
} // namespace Client
} // namespace AW

#endif // APPWHIRR_CLIENT_UTILS_AWAPPLICATIONRUNNER_H
