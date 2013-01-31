#include "applicationlistnetworkresponse.h"
#include <QMap>
#include <QList>
#include <QString>
#include <QScriptValueIterator>
#include <QUrl>

#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"

ApplicationListNetworkResponse::ApplicationListNetworkResponse(const QString& JSONResponse) :
    Background_BaseNetworkResponse(JSONResponse)
{
}

QList<BaseNetworkEntity*> ApplicationListNetworkResponse::receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue) {

    DLog("--- do parse");
    QScriptValueIterator it(JSONValue);

    QList<BaseNetworkEntity*> response;

    // Convert response to array
    while (it.hasNext()) {
        it.next();

        if (it.value().property("applications").isArray()) {
            QScriptValueIterator app_list_it(it.value().property("applications"));
            while (app_list_it.hasNext()) {
                app_list_it.next();

                if (!(app_list_it.value().property("id").toString().isEmpty() &&
                      app_list_it.value().property("name").toString().isEmpty())) {
                    DLog("--- new item created") << app_list_it.value().property("id").toString() << app_list_it.value().property("name").toString();
                    ApplicationBaseInfosNetworkEntity* entity = new ApplicationBaseInfosNetworkEntity;

                    // setup the entity
                    entity->setApplicationID(app_list_it.value().property("id").toString());
                    entity->setApplicationName(app_list_it.value().property("name").toString());
                    entity->setApplicationRating(float(app_list_it.value().property("rating").toNumber()));
                    entity->setApplicationRatingCount(app_list_it.value().property("ratingCount").toInteger());
                    entity->setApplicationReviewCount(app_list_it.value().property("commentsCount").toInteger());

                    // get verion information
                    bool isSuccess = false;
                    if (app_list_it.value().property("version").isArray()) {
                        QScriptValueIterator app_version_it( app_list_it.value().property("version"));
                        while (app_version_it.hasNext()) {
                            app_version_it.next();

                            bool isRequiredBaseInfosFound = true;
                            if (app_version_it.value().property("icon").toString().isEmpty()) {
                                WLog("Icon not found (empty)");
                                isRequiredBaseInfosFound = false;
                            }
                            if(app_version_it.value().property("identifier").toString().isEmpty()) {
                                WLog("Version not found (empty)");
                                isRequiredBaseInfosFound = false;
                            }
                            if(app_version_it.value().property("category").toString().isEmpty()) {
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

                            entity->setAppStatusInStore( (AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum) app_version_it.value().property("status").toUInt32());

                            // Get the url for the encoded string
                            QUrl url(app_version_it.value().property("icon").toString());

                            entity->setApplicationVersion(app_version_it.value().property("identifier").toString());
                            entity->setApplicationIconURL(QString("http://" + url.encodedHost() + url.encodedPath()));
                            entity->setApplicationCategory(app_version_it.value().property("category").toString());
#ifdef Q_WS_WIN
                            entity->setApplicationVersionSummedFlags(app_version_it.value().property("winSummedFlags").toString().toULongLong());
#endif
#ifdef Q_WS_MAC
                            entity->setApplicationVersionSummedFlags(app_version_it.value().property("macSummedFlags").toString().toULongLong());
#endif

                            DLog("Additional infos: ") << app_version_it.value().property("version").toString() << app_version_it.value().property("summedFlages").toInteger();
                        }
                    }

                    if(!isSuccess) {
                        WLog("Required informations not found for app (won't be stored): ") << app_list_it.value().property("id").toString() << app_list_it.value().property("name").toString();
                        delete entity;
                    }
                    else {
                        DLog("--- new item stored");
                        // and save it to the list
                        response.push_back(entity);
                    }
                }
            }
        }
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
