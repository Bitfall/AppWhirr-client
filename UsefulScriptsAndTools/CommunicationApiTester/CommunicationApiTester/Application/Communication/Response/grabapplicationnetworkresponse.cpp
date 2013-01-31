#include "grabapplicationnetworkresponse.h"

#include <QString>
#include <QScriptValue>
#include <QScriptValueIterator>

#include "Application/Communication/Entity/grabapplicationnetworkentity.h"

GrabApplicationNetworkResponse::GrabApplicationNetworkResponse(const QString& JSONResponse) :
    Background_BaseNetworkResponse(JSONResponse)
{
}

QList<BaseNetworkEntity*> GrabApplicationNetworkResponse::receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue) {
    QScriptValueIterator it(JSONValue);

    QList<BaseNetworkEntity*> response;

    // Convert response to array
    while (it.hasNext()) {
        it.next();

        QString givenUrl = it.value().property("url").toString();
        if(!givenUrl.isEmpty()) {
            GrabApplicationNetworkEntity* entity = new GrabApplicationNetworkEntity;

            // setup the entity
            entity->setZippedApplicationURL(givenUrl);
            entity->setRelatedAppVersion(it.value().property("identifier").toString());

            // and save it to the list
            response.push_back(entity);
        }
    }

    return response;
}
