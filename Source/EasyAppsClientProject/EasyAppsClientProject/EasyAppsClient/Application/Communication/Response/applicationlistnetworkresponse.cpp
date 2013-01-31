#include "applicationlistnetworkresponse.h"
#include <QMap>
#include <QList>
#include <QString>
#include <QScriptValueIterator>
#include <QUrl>

#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"

#include "Common/Helpers/platformflagshelper.h"
#include "Common/System/systemprofiler.h"

#include "Common/Json/json.h"

ApplicationListNetworkResponse::ApplicationListNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity*> ApplicationListNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString) {

    DLog("--- do parse");

    QList<AW::Common::Communication::BaseNetworkEntity*> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QVariantList applicationsList = resultMap.value("applications").toList();
    Q_FOREACH(QVariant applicationVar, applicationsList)
    {
        QVariantMap appVarMap = applicationVar.toMap();
        if ( !(appVarMap.value("id").toString().isEmpty() && appVarMap.value("name").toString().isEmpty()) )
        {
            DLog("--- new item created") << appVarMap.value("id").toString() << appVarMap.value("name").toString();
            ApplicationBaseInfosNetworkEntity* entity = new ApplicationBaseInfosNetworkEntity;

            // setup the entity
            entity->setApplicationID(appVarMap.value("id").toString());
            entity->setApplicationName(appVarMap.value("name").toString());
//                    entity->setApplicationRating(float(app_list_it.value().property("rating").toNumber()));
//                    entity->setApplicationRatingCount(app_list_it.value().property("ratingCount").toInteger());
//                    entity->setApplicationReviewCount(app_list_it.value().property("commentsCount").toInteger());

            // get verion information
            bool isSuccess = false;
            if (!appVarMap.value("version").toList().isEmpty()) {
                Q_FOREACH(QVariant version, appVarMap.value("version").toList())
                {
                    QVariantMap versionMap = version.toMap();
                    bool isRequiredBaseInfosFound = true;
                    if (versionMap.value("icon").toString().isEmpty()) {
                        WLog("Icon not found (empty)");
                        isRequiredBaseInfosFound = false;
                    }
//                            if(app_version_it.value().property("identifier").toString().isEmpty()) {
//                                WLog("Version not found (empty)");
//                                isRequiredBaseInfosFound = false;
//                            }
                    if(versionMap.value("category").toString().isEmpty()) {
                        WLog("Category not found (empty)");
                        isRequiredBaseInfosFound = false;
                    }

                    if(!isRequiredBaseInfosFound)
                    {
                        WLog("At least one of the required parameters not found: icon, version or category.");
                        continue;
                    }
                    else {
                        DLog("Required base infos found.");
                        isSuccess = true;
                    }

                    entity->setAppStatusInStore( (AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum) versionMap.value("status").toUInt());

                    // Get the url for the encoded string
                    QUrl url(versionMap.value("icon").toString());

                    // get the proper latest version
                    bool isCompatibleFlaggedAppVersionFound = false;
                    if (!versionMap.value("flaggedApplications").toList().isEmpty())
                    {
                        Q_FOREACH(QVariant flaggedApp, versionMap.value("flaggedApplications").toList())
                        {
                            QVariantMap flaggedAppMap = flaggedApp.toMap();

                            DLog("- flaggedApp");

                            PlatformFlagsHelper::SummedSinglePlatformInformations *flaggedAppPlatformInfo = NULL;
#ifdef Q_WS_WIN
                            quint64 osSubFlags = flaggedAppMap.value("windowsFlags").toString().toULongLong();
                            flaggedAppPlatformInfo = PlatformFlagsHelper::SummedWinPlatformInformations::_createBySummedSubFlags(osSubFlags);
#endif
#ifdef Q_WS_MAC
                            quint64 osSubFlags = flaggedAppMap.value("macFlags").toString().toULongLong();
                            flaggedAppPlatformInfo = PlatformFlagsHelper::SummedMacPlatformInformations::_createBySummedSubFlags(osSubFlags);
#endif
                            if (osSubFlags <= 0)
                                continue;

                            if(flaggedAppPlatformInfo != NULL)
                            {
                                if(SystemProfiler::sharedSystemProfiler()->isThisInfoIsCompatibleWithThisPlatform(flaggedAppPlatformInfo)) {
                                    QString compatibleFlaggedAppVersion = flaggedAppMap.value("version").toString();
                                    DLog("- compatible flagged-app found with version: ") << compatibleFlaggedAppVersion;
                                    entity->setApplicationVersion(compatibleFlaggedAppVersion);
                                    isCompatibleFlaggedAppVersionFound = true;
                                }
                            }
                        }
                    }
                    else {
                        WLog("No flaggedApplications array found!");
                    }
//                            entity->setApplicationVersion(app_version_it.value().property("identifier").toString());
                    if(!isCompatibleFlaggedAppVersionFound) {
                        WLog("No compatible flagged app version found.");
                    }

                    QString iconUrl = QString("https://" + url.encodedHost() + url.encodedPath());
                    DLog("Icon: ") << iconUrl;
                    entity->setApplicationIconURL(iconUrl);
                    entity->setApplicationCategory(versionMap.value("category").toString());
#ifdef Q_WS_WIN
                    entity->setApplicationVersionSummedFlags(versionMap.value("windowsSummedFlags").toString().toULongLong());
#endif
#ifdef Q_WS_MAC
                    entity->setApplicationVersionSummedFlags(versionMap.value("macSummedFlags").toString().toULongLong());
#endif

                    DLog("Additional infos: ") << versionMap.value("version").toString() << versionMap.value("summedFlages").toInt();
                }
            }

            if(!isSuccess) {
                WLog("Required informations not found for app (won't be stored): ") << appVarMap.value("id").toString() << appVarMap.value("name").toString();
                delete entity;
            }
            else {
                DLog("--- new item stored");
                // and save it to the list
                response.push_back(entity);
            }
        }
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
