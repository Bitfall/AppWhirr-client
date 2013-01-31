#include "applicationnetworkresponse.h"

#include <QString>
#include <QUrl>

#include "Application/Communication/Entity/applicationnetworkentity.h"
#include "Application/Communication/Entity/applicationreviewentity.h"
#include "Application/Communication/Entity/applicationscreenshotentity.h"

#include "Common/Json/json.h"

#include "Common/Helpers/platformflagshelper.h"
#include "Common/System/systemprofiler.h"

ApplicationNetworkResponse::ApplicationNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity*> ApplicationNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString)
{
    QList<AW::Common::Communication::BaseNetworkEntity*> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QVariantList applicationsList = resultMap.value("application").toList();

    Q_FOREACH(QVariant applicationVar, applicationsList)
    {
        QVariantMap appMap = applicationVar.toMap();

        if (appMap.value("id").toString().isEmpty() ||
                appMap.value("name").toString().isEmpty()) {
            continue;
        }

        ApplicationDetailInfosNetworkEntity* entity = new ApplicationDetailInfosNetworkEntity;
        DLog("-- new item created");

        // setup the entity
        entity->setApplicationID(appMap.value("id").toString());
        entity->setApplicationName(appMap.value("name").toString());

        // get verion information
        QVariantList appVersionsList = appMap.value("version").toList();
        Q_FOREACH(QVariant appVersionVar, appVersionsList)
        {
            QVariantMap appVersionMap = appVersionVar.toMap();

            //                        DLog("-- before check");
            if (appVersionMap.value("icon").toString().isEmpty() ||
                    //                            app_version_it.value().property("identifier").toString().isEmpty() ||
                    appVersionMap.value("category").toString().isEmpty()
                    )
            {
                WLog("At least one of these required informations is missing: icon, version or category");
                continue;
            }

            //                        DLog("-- after check");

            // Get the url for the encoded string
            QUrl url(appVersionMap.value("icon").toString());


#if 1
            // get the proper latest version
            bool isCompatibleFlaggedAppVersionFound = false;
            QVariantList flaggedAppsList = appVersionMap.value("flaggedApplications").toList();
            if (flaggedAppsList.isEmpty()) {
                WLog("No 'flaggedApplications' found or empty.");
            }
            else {
                Q_FOREACH(QVariant flaggedAppVar, flaggedAppsList)
                {
                    QVariantMap flaggedAppMap = flaggedAppVar.toMap();

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
                    if(flaggedAppPlatformInfo != NULL)
                    {
                        if(SystemProfiler::sharedSystemProfiler()->isThisInfoIsCompatibleWithThisPlatform(flaggedAppPlatformInfo)) {
                            QString compatibleFlaggedAppVersion = flaggedAppMap.value("version").toString();
                            DLog("- compatible flagged-app found with version: ") << compatibleFlaggedAppVersion;
                            entity->setApplicationVersion(compatibleFlaggedAppVersion);
                            isCompatibleFlaggedAppVersionFound = true;
                            break;
                        }
                    }
                }
            }
            //                            entity->setApplicationVersion(app_version_it.value().property("identifier").toString());
            if(!isCompatibleFlaggedAppVersionFound) {
                WLog("No compatible flagged app version found.");
            }
#endif


            entity->setApplicationVersion(appVersionMap.value("version").toString());
            QString appIconUrlAsString = QString("https://") + url.encodedHost() + url.encodedPath();
            DLog("App Icon Url: ") << appIconUrlAsString;
            entity->setApplicationIconURL(appIconUrlAsString);
            entity->setApplicationCategory(appVersionMap.value("category").toString());
            entity->setApplicationSupportURL(appVersionMap.value("supportURL").toString());
            entity->setApplicationDescription(appVersionMap.value("description").toString());
#ifdef Q_WS_WIN
            entity->setApplicationVersionSummedFlags(appVersionMap.value("windowsSummedFlags").toInt());
#endif
#ifdef Q_WS_MAC
            entity->setApplicationVersionSummedFlags(appVersionMap.value("macSummedFlags").toInt());
#endif
            entity->setApplicationWindowsReadableFlags(appVersionMap.value("readableWindowsSummedFlags").toString());
            entity->setApplicationMacReadableFlags(appVersionMap.value("readableMacSummedFlags").toString());

            entity->setTwitterQuery(appVersionMap.value("twitterQuery").toString());
            entity->setSupportUrl(appVersionMap.value("supportURL").toString());
            entity->setApplicationDeveloperURL(appVersionMap.value("developerURL").toString());
            entity->setPrice(appVersionMap.value("price").toString());
            entity->setApplicationSize(appVersionMap.value("hdd").toInt());


            // media - video
            QVariantList moviesList = appVersionMap.value("movies").toList();
            Q_FOREACH(QVariant movieVar, moviesList)
            {
                QVariantMap movieMap = movieVar.toMap();
                entity->addApplicationVideo(movieMap.value("youtubeId").toString());
            }

            // media - screenshot
            QVariantList screenshotsList = appVersionMap.value("screenshots").toList();
            Q_FOREACH(QVariant screenshotVar, screenshotsList)
            {
                QVariantMap screenshotMap = screenshotVar.toMap();
                ApplicationScreenshotEntity* screenshotEntity = new ApplicationScreenshotEntity;
                screenshotEntity->setScreenshotUrl(screenshotMap.value("screenshot").toString());
                screenshotEntity->setThumbnailUrl(screenshotMap.value("thumbnail").toString());
                entity->addApplicationImage(screenshotEntity);
            }
        }


        // and save it to the list
        response.push_back(entity);
        //                DLog("-- new item stored with version") << ((ApplicationNetworkEntity*)entity)->getApplicationVersion();
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
