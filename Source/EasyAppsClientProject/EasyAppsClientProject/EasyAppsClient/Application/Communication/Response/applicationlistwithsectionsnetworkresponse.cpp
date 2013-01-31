#include "applicationlistwithsectionsnetworkresponse.h"

#include <QMap>
#include <QList>
#include <QString>
#include <QUrl>
#include <QVariant>

#include "Application/Communication/Entity/applicationlistwithsectionsnetworkentity.h"

#include "Common/Helpers/platformflagshelper.h"
#include "Common/System/systemprofiler.h"
#include "Common/Json/json.h"

ApplicationListWithSectionsNetworkResponse::ApplicationListWithSectionsNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity*> ApplicationListWithSectionsNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString) {

    DLog("--- do parse");

    QList<AW::Common::Communication::BaseNetworkEntity*> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QVariantList sectionsList = resultMap.value("sections").toList();
    if(sectionsList.isEmpty()) {
        WLog("No sections found!");
    }
    else
    {
        ApplicationListWithSectionsNetworkEntity* entity = new ApplicationListWithSectionsNetworkEntity;

        Q_FOREACH(QVariant sectionVar, sectionsList)
        {
            QVariantMap sectionMap = sectionVar.toMap();

            QString sectionTitle = sectionMap.value("title").toString();
            QUrl sectionBackgroundImageUrl = sectionMap.value("image").toString();
            QString sectionIdentifier = sectionMap.value("identifier").toString();
            QString sectionBackgroundImageUrlAsString = QString("https://") + sectionBackgroundImageUrl.encodedHost() + sectionBackgroundImageUrl.encodedPath();
            ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel sectionModel(sectionTitle, sectionBackgroundImageUrlAsString, sectionIdentifier);

            QVariantList applicationsList = sectionMap.value("applications").toList();
            if(applicationsList.isEmpty()) {
                WLog("No applications found in the section");
            }
            else {
                Q_FOREACH(QVariant applicationVar, applicationsList)
                {
                    QVariantMap applicationMap = applicationVar.toMap();

                    ApplicationBaseInfosNetworkEntityModel appItemModel;

                    if (!(applicationMap.value("id").toString().isEmpty() &&
                          applicationMap.value("name").toString().isEmpty())) {
                        DLog("--- new item created") << applicationMap.value("id").toString() << applicationMap.value("name").toString();


                        // setup the entity
                        appItemModel.setApplicationID(applicationMap.value("id").toString());
                        appItemModel.setApplicationName(applicationMap.value("name").toString());

                        QVariantList appVersionsList = applicationMap.value("version").toList();

                        // get verion information
                        bool isSuccess = false;
                        if(appVersionsList.isEmpty()) {
                            WLog("No app versions found!");
                        }
                        else {

                            Q_FOREACH(QVariant appVersionVar, appVersionsList)
                            {
                                QVariantMap appVersionMap = appVersionVar.toMap();

                                bool isRequiredBaseInfosFound = true;
                                if (appVersionMap.value("icon").toString().isEmpty()) {
                                    WLog("Icon not found (empty)");
                                    isRequiredBaseInfosFound = false;
                                }
                                //                                    if(app_version_it.value().property("identifier").toString().isEmpty()) {
                                //                                        WLog("Version not found (empty)");
                                //                                        isRequiredBaseInfosFound = false;
                                //                                    }
                                if(appVersionMap.value("category").toString().isEmpty()) {
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

                                appItemModel.setAppStatusInStore( (AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum) appVersionMap.value("status").toInt());

                                // Get the url for the encoded string
                                QUrl url(appVersionMap.value("icon").toString());

                                // get the proper latest version
                                QVariantList flaggedAppList = appVersionMap.value("flaggedApplications").toList();
                                bool isCompatibleFlaggedAppVersionFound = false;
                                if(flaggedAppList.isEmpty()) {
                                    WLog("No flagged-app found");
                                }
                                else {
                                    Q_FOREACH(QVariant flaggedAppVar, flaggedAppList)
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
                                                appItemModel.setApplicationVersion(compatibleFlaggedAppVersion);
                                                isCompatibleFlaggedAppVersionFound = true;
                                            }
                                        }
                                    }
                                }
                                //                            appItemModel.setApplicationVersion(app_version_it.value().property("identifier").toString());
                                if(!isCompatibleFlaggedAppVersionFound) {
                                    WLog("No compatible flagged app version found.");
                                }


                                QString appIconUrlAsString = QString("https://") + url.encodedHost() + url.encodedPath();
                                DLog("App Icon Url: ") << appIconUrlAsString;
                                appItemModel.setApplicationIconURL(appIconUrlAsString);
                                appItemModel.setApplicationCategory(appVersionMap.value("category").toString());
#ifdef Q_WS_WIN
                                appItemModel.setApplicationVersionSummedFlags(appVersionMap.value("winSummedFlags").toString().toULongLong());
#endif
#ifdef Q_WS_MAC
                                appItemModel.setApplicationVersionSummedFlags(appVersionMap.value("macSummedFlags").toString().toULongLong());
#endif

                                DLog("Additional infos: ") << appVersionMap.value("version").toString() << appVersionMap.value("summedFlages").toInt();
                            }
                        }

                        if(!isSuccess) {
                            WLog("Required informations not found for app (won't be stored): ") << applicationMap.value("id").toString() << applicationMap.value("name").toString();
                            //                                delete entity;
                        }
                        else {
                            DLog("--- new item stored");
                            // and save it to the list
                            //                                response.push_back(entity);
                            sectionModel.addItemToSection(appItemModel);
                        }
                    }
                }
            }

            if(!sectionModel.isEmpty()) {
                entity->addSection(sectionModel);
            }
        }

        if(!entity->isEmpty()) {
            response.push_back(entity);
        }
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
